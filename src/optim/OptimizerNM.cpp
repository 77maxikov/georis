#include <list>
#include <iostream>
#include "OptSolverNM.h"
#include "common_math.h"

OptSolverNM::OptSolverNM(){
	_rho = 1.0;
	_chi = 2.0;
	_psi = 0.5;
	_sigma = 0.5;
	_tolf = 1e-4;
	_tolx = 1e-4;
	_maxIterCount = 1000;
	_IterCount = 0;
	_maxFunCalls = 1000;
	_FunCalls = 0;
	_optsSet = false;
}
OptSolverNM::~OptSolverNM(){
}
void OptSolverNM::setParam(const std::valarray<double>&parm){
	_rho = parm[0];
	_chi = parm[1];
	_psi = parm[2];
	_sigma = parm[3];
	_tolf = parm[4];
	_tolx = parm[5];
	_optsSet = true;
}
void OptSolverNM::setParam(const std::valarray<int>&parm){
	_maxIterCount = parm[0];
	_maxFunCalls = parm[1];
	_optsSet = true;
}
void OptSolverNM::solve(const MinTaskN &task){
	if (!_optsSet){
		_maxIterCount = 200*task.x0.size();
		_maxFunCalls = _maxIterCount;
		_rho = 1.0;
		_chi = 2.0;
		_psi = 0.5;
		_sigma = 0.5;
		_tolf = 1e-6;
		_tolx = 1e-6;
	}
	else _optsSet = false;
	_X = task.x0;

	//Set up a simplex near the initial guess.
	std::list<v_type> v;
	std::list<s_type> fv;
	// Place input guess in the simplex! (credit L.Pfeffer at Stanford)
	v.push_back(task.x0);
	// ACHTUNG !!! Dims
	fv.push_back((*(task.target))(task.x0));
	// Following improvement suggested by L.Pfeffer at Stanford
	const s_type usual_delta = 0.05; // 5 percent deltas for non-zero terms
	const s_type zero_term_delta = 0.00025; // Even smaller delta for zero elements of x
	for (int k = 0;k<task.x0.size();++k){
		v_type tmp(task.x0);
		if ( !rcomp(tmp(k),0) )
			tmp(k) = (1 + usual_delta)*tmp(k);
		else
			tmp(k) = zero_term_delta;
		s_type f = (*(task.target))(tmp);

		std::list<s_type>::iterator sli = fv.begin();
		std::list<v_type>::iterator vli = v.begin();
		while (sli!=fv.end()){
			if (f<=(*sli)) {fv.insert(sli,f);v.insert(vli,tmp);break;}
			sli++;
			vli++;
		}
		if ( sli == fv.end() ) {fv.push_back(f);v.push_back(tmp);}
	}
	_FunCalls = 0;

	/*	Main algorithm: iterate until
	% (a) the maximum coordinate difference between the current best point and the
	% other points in the simplex is less than or equal to TolX. Specifically,
	% until max(||v2-v1||,||v2-v1||,...,||v(n+1)-v1||) <= TolX,
	% where ||.|| is the infinity-norm, and v1 holds the
	% vertex with the current lowest value; AND
	% (b) the corresponding difference in function values is less than or equal
	% to TolFun. (Cannot use OR instead of AND.)
	% The iteration stops if the maximum number of iterations or function evaluations
	% are exceeded
	*/
	while (_FunCalls < _maxFunCalls && _IterCount < _maxIterCount){
		std::list<s_type>::iterator sli = (++fv.begin());
		std::list<v_type>::iterator vli = (++v.begin());
		s_type fmaxdiff = fabs(*sli - fv.front());
		s_type vmaxdiff = fabs( (*vli)(1) - v.front()(1));
		while (sli!=fv.end()){
			if ( fmaxdiff < fabs(*sli - fv.front()) ) fmaxdiff = fabs(*sli - fv.front());
			for (int k = 0;k<(*vli).size();++k){
				if ( vmaxdiff < fabs( (*vli)(k) - v.front()(k) ) )
					vmaxdiff = fabs( (*vli)(k) - v.front()(k) );
			}

			sli++;
			vli++;
		}
		if (fmaxdiff<_tolf && vmaxdiff < _tolx) break;

		/*if (max(abs(fv(1)-fv(two2np1))) <= max(tolf,10*eps(fv(1))) && ...
		max(max(abs(v(:,two2np1)-v(:,onesn)))) <= max(tolx,10*eps(max(v(:,1))))
		break;
		*/

		// Compute the reflection point
		// xbar = average of the n (NOT n+1) best points
		v_type xbar = v.front();
		if (v.size()>2){
			std::list<v_type>::iterator li = v.begin();
			li++;
			do{
				xbar += *li;
				li++;
				std::list<v_type>::iterator li1 = li;
				li1++;
				if(li1 == v.end()) break;
			} while (1);
		}
		xbar /= task.x0.size();
		v_type xr = xbar*(1.0 + _rho) - v.back()*_rho;
		s_type fxr = (*(task.target))(xr);
		_FunCalls ++;

		if ( fxr < fv.front() ){
			// Calculate the expansion point
			v_type xe = (1 + _rho*_chi)*xbar - _rho*_chi*v.back();
			s_type fxe = (*(task.target))(xe);
			_FunCalls ++;
			if (fxe < fxr){
				v.back() = xe;
				fv.back() = fxe;
			}
			else {
				v.back() = xr;
				fv.back() = fxr;
			}
		}
		else {// fv.front() <= fxr
			if ( fxr < fv.back() ) {
				v.back() = xr;
				fv.back() = fxr;
			}
			else { // fxr >= fv(:,n)
				bool shrink_needed = false;
				// Perform contraction
				if (fxr < fv.back() ){
					// Perform an outside contraction
					v_type xc = (1.0 + _psi*_rho)*xbar - _psi*_rho*v.back();
					s_type fxc = (*(task.target))(xc);
					_FunCalls ++;
					if ( fxc <= fxr ){
						v.back() = xc;
						fv.back() = fxc;
					}
					else//perform a shrink
						shrink_needed = true;
				}
				else{
					// Perform an inside contraction
					v_type xcc = (1.0-_psi)*xbar + _psi*v.back();
					s_type fxcc = (*(task.target))(xcc);
					_FunCalls ++;
					if (fxcc < fv.back() ) {
						v.back() = xcc;
						fv.back() = fxcc;
					}
					else // perform a shrink
						shrink_needed = true;
				}
				if ( shrink_needed ){
					std::list<v_type>::iterator vli = (++v.begin());
					std::list<s_type>::iterator sli = (++fv.begin());
					for ( ;vli!=v.end();vli++,sli++){
						*vli = v.front() + _sigma*(*vli- v.front());
						*sli = (*(task.target))(*vli);
					}
					_FunCalls += v.front().size();
					// Sort them by inserting in right pos os another list
					std::list<v_type> vtmp;
					std::list<s_type> fvtmp;
					vli = v.begin();
					sli = fv.begin();
					vtmp.push_back(*vli);
					fvtmp.push_back(*sli);
					vli++;sli++;
					while (sli != fv.end()){
						std::list<v_type>::iterator vlit = vtmp.begin();
						std::list<s_type>::iterator slit = fvtmp.begin();
						while (slit!=fvtmp.end()){
							if ((*sli)<=(*slit)) {fvtmp.insert(slit,(*sli));vtmp.insert(vlit,(*vli));break;}
							slit++;
							vlit++;
						}
						if (slit == fvtmp.end()) {fvtmp.push_back(*sli);vtmp.push_back(*vli);}
						vli++;sli++;
					}
					fv.swap(fvtmp);
					v.swap(vtmp);
					_IterCount++;
					continue;
				}
			}
		}
		sli = fv.begin();
		vli = v.begin();
		while (sli!=fv.end()){
			if (fv.back() <= (*sli)) {fv.insert(sli,fv.back());v.insert(vli,v.back()); fv.pop_back();v.pop_back(); break;}
			sli++;
			vli++;
		}
		_IterCount++;
	}
	_X = v.front();
}
void OptSolverNM::getSolution(v_type &sol)const{
	sol = _X;
}
void OptSolverNM::getParam(std::valarray<int> &parmi)const{
    parmi.resize(2);
	parmi[0] = _IterCount;
    parmi[1] = _FunCalls;
}

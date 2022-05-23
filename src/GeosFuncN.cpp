#include "GeosFuncN.h"

v_type georis::GeosFuncN::operator()(const v_type &v) {
    for (size_t k = 0; k<m_storage->size(); ++k)
        for (auto par : (*m_storage)[k] )
            *par->pval = v(k);

    v_type e(m_constraints.size());
    for (size_t k = 0; k<m_constraints.size(); ++k)
        e(k) = m_constraints[k]->error();
    return e;
}
spam_type georis::GeosFuncN::getJacob(const v_type &v) {
    // Resize m_pdvals and m_didele if needed
    if ( m_pdvals.rows() != m_constraints.size() ){
        m_pdvals.resize(m_constraints.size(), m_storage->size()); m_pdvals.data().squeeze();
        m_didele.resize(m_constraints.size(), m_storage->size()); m_didele.data().squeeze();

        for ( size_t r = 0; r < m_constraints.size(); ++r)
            for ( size_t c = 0; c < m_storage->size(); ++c ){
                std::vector<DiDelegate> didelev;
                for (auto par : (*m_storage)[c] ){
                    DiDelegate dd = m_constraints[r]->grad(par);
                    if ( dd.empty() ) continue;
                    didelev.push_back(dd);
                }
                if ( !didelev.empty() ){
                    m_didele.insert( r,c ) = didelev;
                    m_pdvals.insert( r,c ) = 0.0;
                }
            }


/*
        std::vector<Eigen::Triplet<double> > valtriplet;
        std::vector<Eigen::Triplet<std::vector<DiDelegate> > > dideletriplet;
        //ACHTUG !!! Estimate number of NNZ ???
        //valtriplet.reserve(estimation_of_entries);
        //dideletriplet.reserve(estimation_of_entries);
        for ( size_t r = 0; r < m_constraints.size(); ++r)
            for ( size_t c = 0; c < m_storage->size(); ++c ){
                std::vector<DiDelegate> didelev;
                for (auto par : (*m_storage)[c] ){
                    DiDelegate dd = m_constraints[r]->grad(par);
                    if ( dd.empty() ) continue;
                    didelev.push_back(dd);
                }
                if ( !didelev.empty() ){
                    dideletriplet.push_back( Eigen::Triplet<std::vector<DiDelegate> >(r,c,didelev) );
                    valtriplet.push_back( Eigen::Triplet<double>(r,c,0.0) );
                }
            }
        m_pdvals.setFromTriplets(valtriplet.begin(), valtriplet.end());
        m_didele.setFromTriplets(dideletriplet.begin(),dideletriplet.end());
        */
    }

    // Set parameters' values
    for (size_t k = 0; k<m_storage->size(); ++k)
        for (auto par : (*m_storage)[k] )
            *par->pval = v(k);
    // Zero nnz elements of m_pdvals
    for (int k=0; k<m_pdvals.outerSize(); ++k){
        for ( spam_type::InnerIterator it(m_pdvals,k); it; ++it){
            it.valueRef() = 0.0;
        }
    }
    // Calc derivatives
    for (int k=0; k<m_pdvals.outerSize(); ++k){
        Eigen::SparseMatrix<std::vector<DiDelegate> >::InnerIterator itd(m_didele,k);
        spam_type::InnerIterator it(m_pdvals,k);
        for ( ; itd; ++itd,++it){
            for ( auto dd: itd.value() )
                it.valueRef() += dd();
        }
    }
    return m_pdvals;
}

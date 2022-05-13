#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "IVisualizer.h"
#include <vector>
#include <iostream>

#include "core.h"
#include "dimline.h"

namespace georis{
class Controller {
public:
    Controller();
    ~Controller();
    void setUI(IVisualizer *);
    void updateView();

    void createObject(ObjectType type, const std::vector<double> &parame); //URok
    void createComposite(CompositeType type, const std::vector<double> &parame); // URok

    void addSelected(bool flag);
    size_t selectByPoint(double x,double y,double precision = 0.01);
    void selectByRect(double x1,double y1,double x2,double y2);
    void resetSelection();

    void deleteSelected(); // UR
    void deleteConstraints(const std::vector<UID>& cids); // UR

    void constrainSelected(ConstraintType type,double parame = 0); //UR
    void toggleAuxSelected();// UR - ok

    void highlightObj(double x,double y,double precision = 0.01);

    void memHighlightsDown(double x,double y);
    void processDrag(double x,double y);
    void memHighlightsUp(double x,double y);// UR
    void resetHighlight();

    void highlightConstrainedBy(UID,bool);

    void createNewSketch();
    void saveTo(const std::string &fname);
    void loadFrom(const std::string &fname);

    void undo();
    void redo();

    void addLayer(); // UR
    void removeLayers(const std::vector<UID>& lid);


    static const char* constrName(ConstraintType ct);
protected:
private:
    IVisualizer *m_ui;
    Core m_core;

    UID m_uidActiveSketch;

    bool m_bIsModified;

    double m_xSel, m_ySel;
    double m_xPrev, m_yPrev;
    std::vector<UID> m_selectedObjs;
    bool m_bAddSelected;

    struct EInfo{
        unsigned status;
        std::string name;
    };

    struct ECInfo:public EInfo{
        ECInfo():type(DCT_NONE),value(0.0),sd(nullptr){}
        DCType type;
        double value;
        std::unique_ptr<sketchDim> sd;

        std::vector<double> param()const{
            return (*sd).param();
        };

    };

    std::map<ObjectType,int> m_lastObjNums;
    std::map<ConstraintType,int> m_lastConstrNums;

    std::map<UID,EInfo> m_objs;
    std::map<UID,ECInfo> m_constrs;

    UID m_memHighlights[2];

    void findObj(unsigned stateMask,std::vector<UID> &res);
    void showSelectionInfo();
    double intFindObjInCirc(double x,double y,double radius,std::vector<UID> &objs,std::vector<double> *pdists = nullptr)const;

    double intFindNearest(double x,double y,std::vector<UID> &objs)const;

    RESCODE intAddConstraint(ConstraintType type,
                          const std::vector<UID> &objects,
                          UID* puid = nullptr,
                          double parame = 0,
                          const std::string &name = std::string());
    RESCODE intRemoveConstraint(UID constrid);

    RESCODE intCreateSimpleObj(ObjectType type, const std::vector<double> &parame,std::vector<UID>* newids = nullptr);

    RESCODE intAddChildObj(ObjectType type, const std::vector<double>& parame,const std::string &name, UID* puid = nullptr,unsigned attributes = 0);
    RESCODE intAddParentObj(ObjectType type, const std::vector<double>& parame,const std::string &name,std::vector<UID>& chuids, UID* puid = nullptr,unsigned attributes = 0);
    RESCODE intRemoveObj(UID);

    RESCODE intToggleAux(UID objid);

    void clearAll(){
        m_memHighlights[0] = NOUID;
        m_memHighlights[1] = NOUID;
        m_lastObjNums.clear();
        m_lastConstrNums.clear();
        m_objs.clear();
        m_constrs.clear();
        m_selectedObjs.clear();
    }

    static const double _sel_precision;
    static bool isChildObj(ObjectType typ){
        if (typ == OT_POINT) return true;
        return false;
    }

    // Undo - redo
    class Command
    {
    public:
       Command(Controller *c):m_controller(c){assert(c);}
       virtual ~Command() {}
       virtual void redo() = 0;
       virtual void undo() = 0;
    protected:
       Controller * m_controller;
    };

    class AddChildObjectCommand: public Command{
    public:
        AddChildObjectCommand(Controller * c,const UID& objid);
        AddChildObjectCommand(Controller * c,
                         const georis::ObjectType type,
                         const std::vector<double> &parame,
                         const std::string &name,
                         const UID& objid ):Command(c),m_type(type),m_parame(parame),m_name(name),m_objid(objid){}
        virtual void redo();
        virtual void undo();
    private:
        georis::ObjectType m_type;
        std::vector<double> m_parame;
        std::string m_name;
        UID m_objid;
    };

    class RemoveChildObjectCommand:public Command{
    public:
        RemoveChildObjectCommand(Controller*c, const UID &);
        virtual void redo();
        virtual void undo();
    private:
        georis::ObjectType m_type;
        std::vector<double> m_parame;
        std::string m_name;
        UID m_objid;
    };

    class AddParentObjectCommand: public Command{
    public:
        AddParentObjectCommand(Controller * c,const UID& objid);
        AddParentObjectCommand(Controller * c,
                         std::vector<UID>&& uids,
                         std::vector<georis::ObjectType> && types,
                         std::vector<std::vector<double> >  &&params,
                         std::vector<std::string> &&names
                         ):Command(c),m_uids(uids),m_types(types),m_params(params),m_names(names){
            assert( uids.size() > 1 );
            assert( uids.size() == types.size() );
            assert( uids.size() == params.size() );
            assert( uids.size() == names.size() );
        }
        virtual void redo();
        virtual void undo();
    private:
        std::vector<UID> m_uids;
        std::vector<georis::ObjectType> m_types;
        std::vector<std::vector<double> >  m_params;
        std::vector<std::string> m_names;
    };
    class RemoveParentObjectCommand:public Command{
    public:
        RemoveParentObjectCommand(Controller*c, const UID &);
        virtual void redo();
        virtual void undo();
    private:
        std::vector<UID> m_uids;
        std::vector<georis::ObjectType> m_types;
        std::vector<std::vector<double> >  m_params;
        std::vector<std::string> m_names;
    };


    class AddConstraintCommand: public Command{
    public:
        AddConstraintCommand(Controller * c,
                         UID constrid,
                         const std::vector<UID>& objids,
                         georis::ConstraintType type,
                         double  param,
                         const std::string& name
                         ):Command(c),m_constrid(constrid),m_objids(objids),m_type(type),m_param(param),m_name(name){}
        virtual void redo();
        virtual void undo();
    private:
        UID  m_constrid;
        std::vector<UID> m_objids;
        georis::ConstraintType m_type;
        double m_param;
        std::string m_name;
    };
    class RemoveConstraintCommand: public Command{
    public:
        RemoveConstraintCommand(Controller * c,
                         UID constrid,
                         const std::vector<UID>& objids,
                         georis::ConstraintType type,
                         double  param,
                         const std::string& name
                         ):Command(c),m_constrid(constrid),m_objids(objids),m_type(type),m_param(param),m_name(name){}
        RemoveConstraintCommand(Controller * c,UID constrid);
        virtual void redo();
        virtual void undo();
    private:
        UID  m_constrid;
        std::vector<UID> m_objids;
        georis::ConstraintType m_type;
        double m_param;
        std::string m_name;
    };
    class ToggleAuxCommand:public Command{
    public:
        ToggleAuxCommand(Controller* c, const std::vector<UID>& oaps):Command(c),m_objids(oaps){}
        virtual void redo();
        virtual void undo();
    private:
        std::vector<UID> m_objids;
    };
    class MoveCommand:public Command{
    public:
        MoveCommand(Controller* c, const std::vector<UID>& objs,double dx,double dy):Command(c),m_objids(objs),m_dx(dx),m_dy(dy){}
        virtual void redo();
        virtual void undo();
    private:
        std::vector<UID> m_objids;
        double m_dx,m_dy;
    };

    class CompositeCommand: public Command{
    public:
        CompositeCommand(Controller* c):Command(c){}
        virtual ~CompositeCommand(){for (auto co:m_commands) delete co;}
        virtual void redo(){for (auto co:m_commands) co->redo();}
        virtual void undo(){
            for (auto co = m_commands.rbegin();co != m_commands.rend();++co )
                (*co)->undo();
        }

        void add(Command*co){
            m_commands.push_back(co);
        }
    private:
        std::vector<Command*> m_commands;
    };
    class UR{
    public:
        UR():m_next2fill(0){};
        void addCommand(Command* c){
            if ( m_next2fill != m_commands.size() ){
                for ( size_t k = m_next2fill; k < m_commands.size();++k)
                    delete m_commands[k];
                m_commands.erase(m_commands.begin() + m_next2fill , m_commands.end() );
            }
            m_commands.push_back(c);
            ++m_next2fill;
        };
        bool canUndo()const{ return m_next2fill != 0;}
        void undo(){
            if ( m_next2fill != 0 ){
                --m_next2fill;
                m_commands[m_next2fill]->undo();
            }
        }
        bool canRedo()const{return ( !m_commands.empty() && m_next2fill < m_commands.size()); }
        void redo(){
            if ( !m_commands.empty() && m_next2fill < m_commands.size() ){
                m_commands[m_next2fill]->redo();
                ++m_next2fill;
            }
        }
        void clear(){
            for ( auto co: m_commands ) delete co;
            m_commands.clear();
        }
    private:
        std::vector<Command*> m_commands;
        size_t m_next2fill;

    };

    UR m_UR;

};
}

#endif // CONTROLLER_H

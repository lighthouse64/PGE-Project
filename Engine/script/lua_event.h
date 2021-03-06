#ifndef LUAEVENT_H
#define LUAEVENT_H


#include <QString>
#include <QVariantList>

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

class LuaEngine;

class LuaEvent{

    friend class LuaEngine;
private:

    std::string m_eventName;
    LuaEngine* m_engine;

    lua_State* getNativeState();

protected:
    std::vector<luabind::object> objList;
public:
    LuaEvent(LuaEngine* engine);
    std::string eventName() const;
    void setEventName(const std::string &eventName);

    template<typename T>
    void addParameter(T par){
        objList.emplace_back(getNativeState(), par);
    }
    void cleanupAllParams();

    static luabind::scope bindToLua();

};




#endif // LUAEVENT_H



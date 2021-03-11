#pragma once

#include "ecs/ecs.hpp"
#include <memory>
#include <vector>

namespace ecs
{
class ECSEngine;
}

namespace opfor
{

///
/// Base class used to create objects that should be kept track of by the engine
///
class EngineObject
{
  protected:
    ecs::ECSEngine *_ecs;

  private:
    std::vector<EngineObject *> _subobjects;
    std::string _editorName;

  public:
    EngineObject(ecs::ECSEngine *engine, std::string const &editorName = "Unnamed Object")
        : _ecs(engine), _editorName(editorName)
    {
    }
    virtual ~EngineObject()
    {
    }

    ///
    /// A method to update the object every frame
    ///
    virtual void Update()
    {
    }

    ///
    /// A method to update the object at a fixed time interval
    ///
    virtual void FixedUpdate()
    {
    }

    ///
    /// Get a vector of non-owned pointers to the object's subobjects
    ///
    std::vector<EngineObject *> GetSubobjects()
    {
        return _subobjects;
    }

    ///
    /// Create a subobject and register it to the engine
    ///
    /// Returns a non-owned pointer
    ///
    template <typename T, typename... ArgTypes> T *CreateSubobject(ArgTypes... args)
    {

        static_assert(std::is_base_of<EngineObject, T>::value);

        // Create object
        auto obj = std::make_unique<T>(std::forward<ArgTypes>(args)...);
        // A non-owned pointer value for the caller
        auto ret = obj.get();

        // Register object
        _subobjects.push_back(std::move(obj));

        return ret;
    }

    ///
    /// Create an Entity from a given list of components
    ///
    template <typename T, typename... ArgTypes> ecs::IEntity<T, ArgTypes...> *CreateEntity()
    {
        return _ecs->GetEntityManager()->CreateEntity<T, ArgTypes...>();
    }

    ///
    /// Get entities that match a list of components given in parameters
    ///
    template <typename... ArgTypes> std::vector<ecs::IEntity<ArgTypes...> *> GetEntities()
    {
        return _ecs->GetEntityManager()->GetEntities<ArgTypes...>();
    }

    std::string const &GetName() const
    {
        return _editorName;
    }
    void SetName(std::string const &name)
    {
        _editorName = name;
    }
};

} // namespace opfor

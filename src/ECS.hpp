#pragma once

#include <bitset>
#include <vector>
#include <iostream>

const int MAX_COMPONENTS = 32;
const int MAX_ENTITIES = 5000;

extern int s_componentCounter;
template <class T>
int getId()
{
    static int s_componentId = s_componentCounter++;
    return s_componentId;
}

typedef unsigned int EntityIndex;
typedef unsigned int EntityVersion;
typedef unsigned long long EntityId;

inline EntityId createEntityId(EntityIndex index, EntityVersion version)
{
    return ((EntityId)index << 32) | ((EntityId)version);
}
inline EntityIndex getEntityIndex(EntityId id)
{
    return id >> 32;
}
inline EntityVersion getEntityVersion(EntityId id)
{
    return (EntityVersion)id;
}
inline bool isEntityValid(EntityId id)
{
    return (id >> 32) != EntityIndex(-1);
}

#define INVALID_ENTITY createEntityId(EntityIndex(-1), 0)

typedef std::bitset<MAX_COMPONENTS> ComponentMask;

struct ComponentPool
{
    ComponentPool(size_t elementsize)
    {
        // We'll allocate enough memory to hold MAX_ENTITIES, each with element size
        elementSize = elementsize;
        pData = new char[elementSize * MAX_ENTITIES];
    }

    ~ComponentPool()
    {
        delete[] pData;
    }

    inline void *get(size_t index)
    {
        return pData + index * elementSize;
    }

    char *pData{nullptr};
    size_t elementSize{0};
};

struct Scene
{
    struct EntityDesc
    {
        EntityId id;
        ComponentMask mask;
    };
    std::vector<EntityDesc> entities;
    std::vector<EntityIndex> freeEntities;
    std::vector<ComponentPool *> componentPools;

    EntityId newEntity()
    {
        if (!freeEntities.empty())
        {
            EntityIndex newIndex = freeEntities.back();
            freeEntities.pop_back();
            EntityId newID = createEntityId(newIndex, getEntityVersion(entities[newIndex].id));
            entities[newIndex].id = newID;
            return entities[newIndex].id;
        }
        entities.push_back({createEntityId(EntityIndex(entities.size()), 0), ComponentMask()});
        return entities.back().id;
    }

    void destroyEntity(EntityId id)
    {
        EntityId newID = createEntityId(EntityIndex(-1), getEntityVersion(id) + 1);
        entities[getEntityIndex(id)].id = newID;
        entities[getEntityIndex(id)].mask.reset();
        freeEntities.push_back(getEntityIndex(id));
    }

    template <typename T>
    T *get(EntityId id)
    {
        if (entities[getEntityIndex(id)].id != id)
            return nullptr;

        int componentId = getId<T>();
        if (!entities[getEntityIndex(id)].mask.test(componentId))
            return nullptr;

        T *pComponent = static_cast<T *>(componentPools[componentId]->get(getEntityIndex(id)));
        return pComponent;
    }

    template <typename T>
    T *assign(EntityId id)
    {

        if (entities[getEntityIndex(id)].id != id)
            return nullptr;

        int componentId = getId<T>();

        if (componentPools.size() <= componentId)
        {
            componentPools.resize(componentId + 1, nullptr);
        }

        if (componentPools[componentId] == nullptr)
        {
            componentPools[componentId] = new ComponentPool(sizeof(T));
        }

        T *pComponent = new (componentPools[componentId]->get(getEntityIndex(id))) T();

        entities[getEntityIndex(id)].mask.set(componentId);

        return pComponent;
    }

    template <typename T>
    void remove(EntityId id)
    {
        // ensures you're not accessing an entity that has been deleted
        if (entities[getEntityIndex(id)].id != id)
            return;

        int componentId = getId<T>();
        entities[getEntityIndex(id)].mask.reset(componentId);
    }
};

template <typename... ComponentTypes>
struct SceneView
{
    Scene *pScene{nullptr};
    ComponentMask componentMask;
    bool all{false};

    SceneView(Scene &scene) : pScene(&scene)
    {
        if (sizeof...(ComponentTypes) == 0)
        {
            all = true;
        }
        else
        {
            // Unpack the template parameters into an initializer list
            int componentIds[] = {0, getId<ComponentTypes>()...};
            for (int i = 1; i < (sizeof...(ComponentTypes) + 1); i++)
                componentMask.set(componentIds[i]);
        }
    }

    struct Iterator
    {
        EntityIndex index;
        Scene *pScene;
        ComponentMask mask;
        bool all{false};

        Iterator(Scene *pScene, EntityIndex index, ComponentMask mask, bool all)
            : pScene(pScene), index(index), mask(mask), all(all) {}

        EntityId operator*() const
        {
            return pScene->entities[index].id;
        }

        bool operator==(const Iterator &other) const
        {
            return index == other.index || index == pScene->entities.size();
        }

        bool operator!=(const Iterator &other) const
        {
            return index != other.index && index != pScene->entities.size();
        }

        bool validIndex()
        {
            return isEntityValid(pScene->entities[index].id) &&
                   (all || mask == (mask & pScene->entities[index].mask));
        }

        Iterator &operator++()
        {
            do
            {
                index++;
            } while (index < pScene->entities.size() && !validIndex());
            return *this;
        }
    };

    const Iterator begin() const
    {
        int firstIndex = 0;
        while (firstIndex < pScene->entities.size() &&
               (componentMask != (componentMask & pScene->entities[firstIndex].mask) || !isEntityValid(pScene->entities[firstIndex].id)))
        {
            firstIndex++;
        }
        return Iterator(pScene, firstIndex, componentMask, all);
    }

    const Iterator end() const
    {
        return Iterator(pScene, EntityIndex(pScene->entities.size()), componentMask, all);
    }
};

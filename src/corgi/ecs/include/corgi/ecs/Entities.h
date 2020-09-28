#pragma once

#include <string>
#include <memory>
#include <map>
#include <optional>
#include <vector>

template <class T>
using Optional = std::optional<T>;

namespace corgi
{
    class Entity;
    class Scene;

    template <class T>
    using Reference = std::reference_wrapper<T>;

    template<class T>
    using UniquePtr = std::unique_ptr<T>;

    template<class T, class U>
    using Map = std::map<T, U>;

    template<class T>
    using Vector = std::vector<T>;

    using String = std::string;

/*!
 * @brief   Stores the entities. Entities are tied to a scene
 *
 */
class Entities
{
    friend class Scene;

public:

	// Lifecycle

    ~Entities();
	

    // Functions

    /*!
     * @brief   Constructs a new entity in place at root level
     */
    Entity& emplace(const String& name)noexcept;

    /*!
     * @brief  Copy constructs a new entity in place at root level by using @a entity content
     */
    Entity& emplace(const Entity& entity) noexcept;

    /*!
     * @brief   Move constructs a new entity and move the content of @a entity inside it
     */
    Entity& emplace(Entity&& entity) noexcept;

    /*!
     * @brief	Tries to find an entity called "name" inside the scene
     *			Returns a pointer to the entity if founded, returns nullptr
     * 			otherwise
     */
    [[nodiscard]] Optional<Reference<Entity>> find(const String& name) noexcept;
    [[nodiscard]] Optional<Reference<const Entity>> find(const String& name)const noexcept;

    /*!
     * @brief   Tries to find a reference to the entity with an id equals to @a id
    *           If no e
     */
    Optional<Reference<Entity>> operator[](int id) noexcept;


    void register_entity(int id, Entity& e)
    {
        entities_.emplace(id, e);
    }

    /*!
     * @brief   Tries to find a reference to an entity with an id equals to @a id
     */
    Optional<Reference<const Entity>> operator[](int id) const noexcept;

	Entity& root()
	{
        return *root_;
	}

protected:

    // Lifecycle

    /*!
     * @brief   The entity manager is tightly coupled to the scene,
     *          and should only be contained inside a scene object
     */
    Entities(Scene& scene);

private:

    // Member variables

    Scene& scene_;

    // Entities at the root level
    UniquePtr<Entity> root_;

    Map<int, Reference<Entity>> entities_;
};
}
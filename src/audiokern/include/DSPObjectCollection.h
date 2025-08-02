#pragma once

#include "dsp_types.h"
#include <cstdlib>
#include <memory>

/**
 * @brief A template collection for managing DSP object pointers
 *
 * DSPObjectCollection manages pointers to DSP objects with automatic
 * memory cleanup and reference-based access. The collection takes ownership
 * of the pointed-to objects and handles their destruction automatically.
 *
 * Usage:
 * 
 * - Call add() to insert object pointers into the collection
 * - Use operator[] for direct reference access to objects
 * - Call size() to get the current number of objects
 * - Call clear() to remove and delete all objects
 * 
 * The collection automatically deletes all managed objects when destroyed
 * or when clear() is called, ensuring no memory leaks.
 * 
 * Example:
 * @code
 * DSPObjectCollection<ExcampleClass> processors;
 * 
 * ExcampleClass* processor = new ExcampleClass();
 * processors.add(processor);
 * 
 * // Access by reference, not pointer
 * processors[0].setValue(1.0f);
 * 
 * // Automatic cleanup when collection is destroyed
 * @endcode
 */
template<typename T>
class DSPObjectCollection
{
public:
    DSPObjectCollection()
        : objects(std::make_unique<T*[]>(initialCapacity))
        , objectCount(0)
        , capacity(initialCapacity)
    {
    }

    ~DSPObjectCollection()
    {
        clear();
    }

    /**
     * @brief Adds an object pointer to the collection
     * 
     * Takes ownership of the provided pointer. The collection will
     * automatically delete the object when the collection is destroyed
     * or cleared.
     *
     * @param objectPtr Pointer to the object to manage
     * @return Reference to the managed object
     */
    T& add(T* objectPtr)
    {
        if (objectCount >= capacity)
        {
            expandCapacity();
        }
        
        objects[objectCount++] = objectPtr;
        return *objectPtr;
    }

    /**
     * @brief Accesses an object by index
     * 
     * Provides direct reference access to managed objects.
     * Index bounds are not checked for performance reasons.
     *
     * @param index Zero-based index of the object to access
     * @return Reference to the object at the specified index
     */
    T& operator[](size_t index)
    {
        return *objects[index];
    }

    /**
     * @brief Accesses an object by index (const version)
     * 
     * Provides const reference access to managed objects.
     *
     * @param index Zero-based index of the object to access
     * @return Const reference to the object at the specified index
     */
    const T& operator[](size_t index) const
    {
        return *objects[index];
    }

    /**
     * @brief Returns the number of objects in the collection
     * 
     * @return Current number of objects stored in the collection
     */
    size_t size() const
    {
        return objectCount;
    }

    /**
     * @brief Removes and deletes all objects from the collection
     * 
     * Deletes all managed objects and clears the collection.
     * All references to objects become invalid after this call.
     */
    void clear()
    {
        for (size_t i = 0; i < objectCount; ++i)
        {
            delete objects[i];
        }
        objectCount = 0;
    }

    /**
     * @brief Checks if the collection is empty
     * 
     * @return True if the collection contains no objects, false otherwise
     */
    bool empty() const
    {
        return objectCount == 0;
    }

    /**
     * @brief Gets the raw pointer at the specified index
     * 
     * Provides access to the underlying pointer for cases where
     * pointer access is specifically needed.
     *
     * @param index Zero-based index of the pointer to access
     * @return Pointer to the object at the specified index
     */
    T* getPointer(size_t index)
    {
        return objects[index];
    }

private:
    /// @brief Internal storage for object pointers
    std::unique_ptr<T*[]> objects;
    
    /// @brief Current number of objects in the collection
    size_t objectCount;
    
    /// @brief Current capacity of the collection
    size_t capacity;
    
    /// @brief Initial and growth increment for collection capacity
    static constexpr size_t initialCapacity = 16;

    /**
     * @brief Expands collection capacity when needed
     * 
     * Doubles the current capacity and relocates existing pointers.
     */
    void expandCapacity()
    {
        size_t newCapacity = capacity * 2;
        auto newObjects = std::make_unique<T*[]>(newCapacity);
        
        // Copy existing pointers to new storage
        for (size_t i = 0; i < objectCount; ++i)
        {
            newObjects[i] = objects[i];
        }
        
        objects = std::move(newObjects);
        capacity = newCapacity;
    }
};
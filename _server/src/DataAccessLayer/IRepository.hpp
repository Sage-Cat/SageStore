/**
 * @file IRepository.hpp
 * @brief Header file for the IRepository interface.
 */

#ifndef IREPOSITORY_HPP
#define IREPOSITORY_HPP

/**
 * @interface IRepository
 * @brief Interface for repositories.
 *
 * This interface defines the basic CRUD (Create, Read, Update, Delete) functionalities
 * that any repository should implement.
 */
class IRepository
{
public:
    /**
     * @brief Destructor for the IRepository class.
     */
    virtual ~IRepository() = default;

    /**
     * @brief Insert a new item into the repository.
     *
     * This function is used to insert a new item into the repository.
     */
    virtual void insert() = 0;

    /**
     * @brief Update an existing item in the repository.
     *
     * This function is used to update an existing item in the repository.
     */
    virtual void update() = 0;

    /**
     * @brief Remove an item from the repository.
     *
     * This function is used to remove an item from the repository.
     */
    virtual void remove() = 0;

    /**
     * @brief Query the repository for data.
     *
     * This function is used to query the repository for data.
     */
    virtual void query() = 0;
};

#endif // IREPOSITORY_HPP

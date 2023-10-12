class IRepository
{
public:
    /**
     * @brief Destructor for the IRepository class.
     */
    virtual ~IRepository() = default;

    /**
     * @brief Insert a new item into the repository.
     */
    virtual void insert() = 0;

    /**
     * @brief Update an existing item in the repository.
     */
    virtual void update() = 0;

    /**
     * @brief Remove an item from the repository.
     */
    virtual void remove() = 0;

    /**
     * @brief Query the repository for data.
     */
    virtual void query() = 0;
};
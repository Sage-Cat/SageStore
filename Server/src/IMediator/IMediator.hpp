class IMediator
{
public:
    /**
     * @brief Destructor for the IMediator class.
     */
    virtual ~IMediator() = default;

    /**
     * @brief Mediate a communication between components or objects.
     */
    virtual void mediate() = 0;
};
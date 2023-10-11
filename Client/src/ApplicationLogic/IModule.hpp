class IModule
{
public:
    virtual ~IModule() = default;

    virtual void init() = 0;
};

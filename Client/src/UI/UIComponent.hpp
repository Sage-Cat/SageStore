class UiComponent
{
public:
    virtual ~UiComponent() = default;

    virtual void render() = 0;
};
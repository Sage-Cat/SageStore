#include <QString>

class IDataSerialization
{
public:
    virtual ~IDataSerialization() = default;

    virtual QString serialize() = 0;
    virtual void deserialize(const QString &data) = 0;
};

#include <QString>

struct Role
{
    QString id;
    QString name;
    Role() = default;
    Role(const QString &id, const QString &name) : id{id},
                                                   name{name}
    {
    }
};
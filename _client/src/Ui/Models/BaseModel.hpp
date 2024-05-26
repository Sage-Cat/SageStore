#pragma once

#include <QObject>

/**
 * @interface BaseModel
 * @brief Interface for models.
 */
class BaseModel : public QObject {
    Q_OBJECT

public:
    explicit BaseModel(QObject *parent = nullptr);
    virtual ~BaseModel();
};

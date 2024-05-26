#pragma once

#include <QObject>

class BaseModel;

/**
 * @interface BaseModel
 * @brief Interface for models.
 */
class BaseViewModel : public QObject {
    Q_OBJECT

public:
    explicit BaseViewModel(BaseModel &model, QObject *parent = nullptr);
    virtual ~BaseViewModel();

signals:
    void errorOccurred(const QString &errorMessage);
};

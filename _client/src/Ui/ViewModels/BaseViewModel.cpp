#include "BaseViewModel.hpp"

#include "Ui/Models/BaseModel.hpp"

BaseViewModel::BaseViewModel(BaseModel &model, QObject *parent) : QObject(parent)
{
    connect(this, &BaseViewModel::errorOccurred, &model, &BaseModel::errorOccurred);
}

BaseViewModel::~BaseViewModel() = default;

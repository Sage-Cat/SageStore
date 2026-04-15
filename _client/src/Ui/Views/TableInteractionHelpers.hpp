#pragma once

#include "Ui/UiScale.hpp"

#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QAbstractSpinBox>
#include <QApplication>
#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QEvent>
#include <QFrame>
#include <QHeaderView>
#include <QLineEdit>
#include <QLayout>
#include <QListView>
#include <QMouseEvent>
#include <QPointer>
#include <QLocale>
#include <QProxyStyle>
#include <QSignalBlocker>
#include <QSizePolicy>
#include <QSpinBox>
#include <QStyledItemDelegate>
#include <QTableWidget>
#include <QTimer>
#include <QVariant>
#include <algorithm>
#include <functional>
#include <limits>
#include <utility>

class TableRowDoubleClickEditSupport : public QObject {
public:
    TableRowDoubleClickEditSupport(QTableWidget *table, std::function<void(int)> handler)
        : QObject(table), m_table(table), m_handler(std::move(handler))
    {
        Q_ASSERT(m_table != nullptr);
        m_table->viewport()->installEventFilter(this);
    }

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (m_table != nullptr && watched == m_table->viewport() &&
            event->type() == QEvent::MouseButtonDblClick) {
            const auto *mouseEvent = static_cast<QMouseEvent *>(event);
            const QModelIndex index = m_table->indexAt(mouseEvent->position().toPoint());
            if (index.isValid()) {
                m_table->selectRow(index.row());
                if (m_handler) {
                    m_handler(index.row());
                }
                return true;
            }
        }

        return QObject::eventFilter(watched, event);
    }

private:
    QPointer<QTableWidget> m_table;
    std::function<void(int)> m_handler;
};

inline void installRowDoubleClickEditSupport(QTableWidget *table,
                                             std::function<void(int)> handler)
{
    Q_ASSERT(table != nullptr);
    new TableRowDoubleClickEditSupport(table, std::move(handler));
}

inline QTableWidgetItem *createEditableTableItem(const QString &text = {})
{
    auto *item = new QTableWidgetItem(text);
    item->setData(Qt::UserRole, text);
    return item;
}

inline QTableWidgetItem *createReadOnlyTableItem(const QString &text = {})
{
    auto *item = createEditableTableItem(text);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    return item;
}

inline QTableWidgetItem *createComboTableItem(const QString &text, const QVariant &value,
                                              bool editable = true)
{
    auto *item = editable ? createEditableTableItem(text) : createReadOnlyTableItem(text);
    item->setData(Qt::UserRole, value);
    return item;
}

inline void restoreItemText(QTableWidget *table, QTableWidgetItem *item, const QString &text)
{
    Q_ASSERT(table != nullptr);
    Q_ASSERT(item != nullptr);
    const QSignalBlocker blocker(table);
    item->setText(text);
}

inline QString trackedItemText(const QTableWidgetItem *item)
{
    return item != nullptr ? item->data(Qt::UserRole).toString() : QString();
}

inline bool restoreIfUnchanged(QTableWidget *table, QTableWidgetItem *item,
                               const QString &normalizedValue)
{
    Q_ASSERT(table != nullptr);
    Q_ASSERT(item != nullptr);

    if (trackedItemText(item) != normalizedValue) {
        return false;
    }

    if (item->text() != normalizedValue) {
        restoreItemText(table, item, normalizedValue);
    }
    return true;
}

inline void beginTableItemEdit(QTableWidget *table, int row, int column)
{
    Q_ASSERT(table != nullptr);
    if (row < 0 || column < 0) {
        return;
    }

    if (auto *item = table->item(row, column); item != nullptr) {
        table->setCurrentItem(item);
        table->editItem(item);
    }
}

inline int defaultInlineEditorRowHeight()
{
    return UiScale::scalePx(36);
}

class ComboPopupProxyStyle : public QProxyStyle {
public:
    explicit ComboPopupProxyStyle(const QString &baseStyleKey) : QProxyStyle(baseStyleKey) {}

    int styleHint(StyleHint hint, const QStyleOption *option = nullptr,
                  const QWidget *widget = nullptr,
                  QStyleHintReturn *returnData = nullptr) const override
    {
        switch (hint) {
        case QStyle::SH_ComboBox_Popup:
            return 0;
        case QStyle::SH_ComboBox_PopupFrameStyle:
            return QFrame::NoFrame;
        default:
            return QProxyStyle::styleHint(hint, option, widget, returnData);
        }
    }
};

inline void configureComboPopupContainer(QWidget *popupWindow, int minimumWidth,
                                         const char *containerProperty)
{
    if (popupWindow == nullptr) {
        return;
    }

    Q_ASSERT(containerProperty != nullptr);
    popupWindow->setProperty(containerProperty, true);
    popupWindow->setAttribute(Qt::WA_StyledBackground, true);
    popupWindow->setContentsMargins(0, 0, 0, 0);
    popupWindow->setMinimumHeight(0);
    popupWindow->setMaximumHeight(QWIDGETSIZE_MAX);
    popupWindow->setMinimumWidth(minimumWidth);
    if (auto *layout = popupWindow->layout(); layout != nullptr) {
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
    }
    if (auto *frame = qobject_cast<QFrame *>(popupWindow); frame != nullptr) {
        frame->setFrameShape(QFrame::NoFrame);
        frame->setLineWidth(0);
        frame->setMidLineWidth(0);
    }
}

inline void configureTableComboPopupContainer(QWidget *popupWindow, int minimumWidth)
{
    configureComboPopupContainer(popupWindow, minimumWidth, "tableComboPopupContainer");
}

class PopupListComboBox : public QComboBox {
public:
    PopupListComboBox(const char *popupContainerProperty, QWidget *parent = nullptr)
        : QComboBox(parent), m_popupContainerProperty(popupContainerProperty)
    {
        Q_ASSERT(m_popupContainerProperty != nullptr);
    }

protected:
    void showPopup() override
    {
        for (QWidget *topLevel : QApplication::topLevelWidgets()) {
            if (topLevel != nullptr && topLevel->isVisible() &&
                topLevel->property(m_popupContainerProperty).toBool()) {
                topLevel->hide();
            }
        }

        QComboBox::showPopup();

        if (auto *popupWindow = popupContainer(); popupWindow != nullptr) {
            configureComboPopupContainer(popupWindow, width(), m_popupContainerProperty);
            popupWindow->updateGeometry();
            popupWindow->update();
        }
    }

private:
    QWidget *popupContainer() const
    {
        auto *popupView = view();
        return popupView != nullptr ? popupView->window() : nullptr;
    }

    const char *m_popupContainerProperty;
};

class FormComboBox : public PopupListComboBox {
public:
    explicit FormComboBox(QWidget *parent = nullptr)
        : PopupListComboBox("comboPopupContainer", parent)
    {
    }
};

class TableComboBox : public PopupListComboBox {
public:
    explicit TableComboBox(QWidget *parent = nullptr)
        : PopupListComboBox("tableComboPopupContainer", parent)
    {
    }
};

inline void styleTableEditorWidget(QWidget *editor, const QString &objectName = {},
                                   bool persistent = false)
{
    Q_ASSERT(editor != nullptr);
    editor->setObjectName(objectName);
    editor->setProperty("tableEditor", true);
    editor->setProperty("tablePersistentEditor", persistent);
    editor->setAttribute(Qt::WA_StyledBackground, true);
    editor->setAutoFillBackground(false);
    editor->setFocusPolicy(Qt::StrongFocus);
    editor->setContentsMargins(0, 0, 0, 0);
    editor->setMinimumHeight(0);
    editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

inline void configureComboPopupView(QComboBox *combo, const char *popupProperty)
{
    Q_ASSERT(combo != nullptr);
    Q_ASSERT(popupProperty != nullptr);
    auto *popupStyle = new ComboPopupProxyStyle(QApplication::style()->name());
    popupStyle->setParent(combo);
    combo->setStyle(popupStyle);
    auto *popupView = new QListView(combo);
    popupView->setProperty(popupProperty, true);
    popupView->setFrameShape(QFrame::NoFrame);
    popupView->setUniformItemSizes(true);
    popupView->setSpacing(0);
    popupView->setSelectionBehavior(QAbstractItemView::SelectRows);
    popupView->setSelectionMode(QAbstractItemView::SingleSelection);
    popupView->setAlternatingRowColors(false);
    popupView->setMouseTracking(true);
    popupView->setTextElideMode(Qt::ElideRight);
    popupView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    popupView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    popupView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    popupView->setContentsMargins(0, 0, 0, 0);
    combo->setView(popupView);
}

inline void configureFormComboBox(QComboBox *combo, const QString &objectName = {})
{
    Q_ASSERT(combo != nullptr);
    if (!objectName.isEmpty()) {
        combo->setObjectName(objectName);
    }

    combo->setAttribute(Qt::WA_StyledBackground, true);
    combo->setEditable(false);
    combo->setInsertPolicy(QComboBox::NoInsert);
    combo->setMaxVisibleItems(12);
    combo->setMinimumContentsLength(1);
    combo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    configureComboPopupView(combo, "comboPopup");
}

inline void configureTableComboBox(QComboBox *combo, const QString &objectName = {})
{
    Q_ASSERT(combo != nullptr);
    styleTableEditorWidget(combo, objectName, true);
    combo->setEditable(false);
    combo->setFrame(false);
    combo->setInsertPolicy(QComboBox::NoInsert);
    combo->setMaxVisibleItems(12);
    combo->setMinimumContentsLength(1);
    combo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    combo->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    configureComboPopupView(combo, "tableComboPopup");
}

class TableInlineEditorDelegateBase : public QStyledItemDelegate {
public:
    explicit TableInlineEditorDelegateBase(QString editorObjectName = {}, QObject *parent = nullptr)
        : QStyledItemDelegate(parent), m_editorObjectName(std::move(editorObjectName))
    {
    }

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override
    {
        QSize hint = QStyledItemDelegate::sizeHint(option, index);
        hint.setHeight(std::max(hint.height(), defaultInlineEditorRowHeight()));
        return hint;
    }

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                              const QModelIndex &) const override
    {
        if (editor != nullptr) {
            const QRect editorRect = option.rect.adjusted(1, 1, -1, -1);
            editor->setGeometry(editorRect);
            editor->setMinimumHeight(editorRect.height());
            editor->setMaximumHeight(editorRect.height());
        }
    }

protected:
    void prepareEditor(QWidget *editor) const
    {
        styleTableEditorWidget(editor, m_editorObjectName);
    }

    void selectAllLater(QWidget *editor) const
    {
        if (editor == nullptr) {
            return;
        }

        QTimer::singleShot(0, editor, [editor]() {
            if (auto *lineEdit = qobject_cast<QLineEdit *>(editor); lineEdit != nullptr) {
                lineEdit->selectAll();
            } else if (auto *spinBox = qobject_cast<QAbstractSpinBox *>(editor);
                       spinBox != nullptr) {
                spinBox->selectAll();
            }
        });
    }

private:
    QString m_editorObjectName;
};

class TableLineEditDelegate : public TableInlineEditorDelegateBase {
public:
    using Normalizer = std::function<QString(const QString &)>;

    TableLineEditDelegate(QString editorObjectName = {}, Normalizer normalizer = {},
                          QObject *parent = nullptr)
        : TableInlineEditorDelegateBase(std::move(editorObjectName), parent),
          m_normalizer(std::move(normalizer))
    {
    }

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &,
                          const QModelIndex &) const override
    {
        auto *editor = new QLineEdit(parent);
        editor->setFrame(false);
        editor->setClearButtonEnabled(false);
        prepareEditor(editor);
        return editor;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const override
    {
        auto *lineEdit = qobject_cast<QLineEdit *>(editor);
        if (lineEdit == nullptr) {
            return;
        }

        lineEdit->setText(index.data(Qt::EditRole).toString());
        selectAllLater(lineEdit);
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override
    {
        auto *lineEdit = qobject_cast<QLineEdit *>(editor);
        if (lineEdit == nullptr || model == nullptr) {
            return;
        }

        const QString text = m_normalizer ? m_normalizer(lineEdit->text()) : lineEdit->text();
        if (index.data(Qt::EditRole).toString() == text) {
            return;
        }
        model->setData(index, text, Qt::EditRole);
    }

private:
    Normalizer m_normalizer;
};

class TableSpinBoxDelegate : public TableInlineEditorDelegateBase {
public:
    TableSpinBoxDelegate(int minimum, int maximum, QString editorObjectName = {},
                         QObject *parent = nullptr)
        : TableInlineEditorDelegateBase(std::move(editorObjectName), parent), m_minimum(minimum),
          m_maximum(maximum)
    {
    }

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &,
                          const QModelIndex &) const override
    {
        auto *editor = new QSpinBox(parent);
        editor->setFrame(false);
        editor->setButtonSymbols(QAbstractSpinBox::NoButtons);
        editor->setRange(m_minimum, m_maximum);
        editor->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        prepareEditor(editor);
        return editor;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const override
    {
        auto *spinBox = qobject_cast<QSpinBox *>(editor);
        if (spinBox == nullptr) {
            return;
        }

        bool ok = false;
        int value = index.data(Qt::EditRole).toString().toInt(&ok);
        if (!ok) {
            value = m_minimum;
        }

        spinBox->setValue(std::clamp(value, m_minimum, m_maximum));
        selectAllLater(spinBox);
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override
    {
        auto *spinBox = qobject_cast<QSpinBox *>(editor);
        if (spinBox == nullptr || model == nullptr) {
            return;
        }

        spinBox->interpretText();
        const QString value = QString::number(spinBox->value());
        if (index.data(Qt::EditRole).toString() == value) {
            return;
        }
        model->setData(index, value, Qt::EditRole);
    }

private:
    int m_minimum;
    int m_maximum;
};

class TableDoubleSpinBoxDelegate : public TableInlineEditorDelegateBase {
public:
    TableDoubleSpinBoxDelegate(double minimum, double maximum, int decimals, double singleStep,
                               QString editorObjectName = {}, QObject *parent = nullptr)
        : TableInlineEditorDelegateBase(std::move(editorObjectName), parent), m_minimum(minimum),
          m_maximum(maximum), m_decimals(decimals), m_singleStep(singleStep)
    {
    }

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &,
                          const QModelIndex &) const override
    {
        auto *editor = new QDoubleSpinBox(parent);
        editor->setFrame(false);
        editor->setButtonSymbols(QAbstractSpinBox::NoButtons);
        editor->setRange(m_minimum, m_maximum);
        editor->setDecimals(m_decimals);
        editor->setSingleStep(m_singleStep);
        editor->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        editor->setLocale(QLocale::c());
        prepareEditor(editor);
        return editor;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const override
    {
        auto *spinBox = qobject_cast<QDoubleSpinBox *>(editor);
        if (spinBox == nullptr) {
            return;
        }

        bool ok = false;
        double value = index.data(Qt::EditRole).toString().toDouble(&ok);
        if (!ok) {
            value = m_minimum;
        }

        spinBox->setValue(std::clamp(value, m_minimum, m_maximum));
        selectAllLater(spinBox);
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override
    {
        auto *spinBox = qobject_cast<QDoubleSpinBox *>(editor);
        if (spinBox == nullptr || model == nullptr) {
            return;
        }

        spinBox->interpretText();
        const QString value = QString::number(spinBox->value(), 'f', m_decimals);
        if (index.data(Qt::EditRole).toString() == value) {
            return;
        }
        model->setData(index, value, Qt::EditRole);
    }

private:
    double m_minimum;
    double m_maximum;
    int m_decimals;
    double m_singleStep;
};

class TableDateEditDelegate : public TableInlineEditorDelegateBase {
public:
    explicit TableDateEditDelegate(QString format = QStringLiteral("yyyy-MM-dd"),
                                   QString editorObjectName = {}, QObject *parent = nullptr)
        : TableInlineEditorDelegateBase(std::move(editorObjectName), parent),
          m_format(std::move(format))
    {
    }

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &,
                          const QModelIndex &) const override
    {
        auto *editor = new QDateEdit(parent);
        editor->setFrame(false);
        editor->setButtonSymbols(QAbstractSpinBox::NoButtons);
        editor->setCalendarPopup(false);
        editor->setDisplayFormat(m_format);
        editor->setAlignment(Qt::AlignCenter);
        prepareEditor(editor);
        return editor;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const override
    {
        auto *dateEdit = qobject_cast<QDateEdit *>(editor);
        if (dateEdit == nullptr) {
            return;
        }

        QDate date = QDate::fromString(index.data(Qt::EditRole).toString(), m_format);
        if (!date.isValid()) {
            date = QDate::currentDate();
        }
        dateEdit->setDate(date);
        selectAllLater(dateEdit);
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override
    {
        auto *dateEdit = qobject_cast<QDateEdit *>(editor);
        if (dateEdit == nullptr || model == nullptr) {
            return;
        }

        const QString value = dateEdit->date().toString(m_format);
        if (index.data(Qt::EditRole).toString() == value) {
            return;
        }
        model->setData(index, value, Qt::EditRole);
    }

private:
    QString m_format;
};

struct TableComboChoice {
    QString label;
    QVariant value;
};

class TablePersistentComboSupport : public QObject {
public:
    explicit TablePersistentComboSupport(QTableWidget *table) : QObject(table), m_table(table)
    {
        Q_ASSERT(m_table != nullptr);
        auto *model = m_table->model();
        Q_ASSERT(model != nullptr);

        connect(model, &QAbstractItemModel::modelReset, this, [this]() { scheduleRefresh(); });
        connect(model, &QAbstractItemModel::layoutChanged, this, [this]() { scheduleRefresh(); });
        connect(model, &QAbstractItemModel::rowsInserted, this,
                [this](const QModelIndex &, int, int) { scheduleRefresh(); });
        connect(model, &QAbstractItemModel::rowsRemoved, this,
                [this](const QModelIndex &, int, int) { scheduleRefresh(); });
        connect(model, &QAbstractItemModel::columnsInserted, this,
                [this](const QModelIndex &, int, int) { scheduleRefresh(); });
        connect(model, &QAbstractItemModel::columnsRemoved, this,
                [this](const QModelIndex &, int, int) { scheduleRefresh(); });
        connect(model, &QAbstractItemModel::dataChanged, this,
                [this](const QModelIndex &topLeft, const QModelIndex &bottomRight,
                       const QList<int> &) {
                    if (!topLeft.isValid() || !bottomRight.isValid()) {
                        scheduleRefresh();
                        return;
                    }

                    if (hasRegisteredColumnInRange(topLeft.column(), bottomRight.column())) {
                        scheduleRefresh();
                    }
                });
    }

    void addColumn(int column)
    {
        if (!m_columns.contains(column)) {
            m_columns.push_back(column);
        }
        scheduleRefresh();
    }

    void refreshNow()
    {
        m_refreshScheduled = false;
        refreshEditors();
    }

private:
    void scheduleRefresh()
    {
        if (m_refreshScheduled || m_table == nullptr) {
            return;
        }

        m_refreshScheduled = true;
        QTimer::singleShot(0, this, [this]() {
            m_refreshScheduled = false;
            refreshEditors();
        });
    }

    bool hasRegisteredColumnInRange(int left, int right) const
    {
        return std::any_of(m_columns.cbegin(), m_columns.cend(), [left, right](int column) {
            return column >= left && column <= right;
        });
    }

    void refreshEditors()
    {
        if (m_table == nullptr) {
            return;
        }

        for (int row = 0; row < m_table->rowCount(); ++row) {
            for (const int column : m_columns) {
                auto *item = m_table->item(row, column);
                if (item == nullptr) {
                    continue;
                }

                const bool editable = item->flags().testFlag(Qt::ItemIsEditable);
                if (editable) {
                    if (!m_table->isPersistentEditorOpen(item)) {
                        m_table->openPersistentEditor(item);
                    }
                } else if (m_table->isPersistentEditorOpen(item)) {
                    m_table->closePersistentEditor(item);
                }
            }
        }
    }

    QPointer<QTableWidget> m_table;
    QVector<int> m_columns;
    bool m_refreshScheduled = false;
};

inline TablePersistentComboSupport *ensurePersistentComboSupport(QTableWidget *table)
{
    Q_ASSERT(table != nullptr);
    if (auto *supportObject =
            table->findChild<QObject *>(QStringLiteral("__tablePersistentComboSupport"));
        supportObject != nullptr) {
        return static_cast<TablePersistentComboSupport *>(supportObject);
    }

    auto *support = new TablePersistentComboSupport(table);
    support->setObjectName(QStringLiteral("__tablePersistentComboSupport"));
    return support;
}

inline void refreshPersistentComboEditors(QTableWidget *table)
{
    ensurePersistentComboSupport(table)->refreshNow();
}

class TableComboDelegate : public QStyledItemDelegate {
public:
    using ChoicesProvider = std::function<QVector<TableComboChoice>(const QModelIndex &)>;
    using CommitHandler = std::function<void(const QModelIndex &, const TableComboChoice &)>;

    TableComboDelegate(QTableWidget *table, QString editorObjectName, ChoicesProvider choicesProvider,
                       CommitHandler commitHandler = {}, QObject *parent = nullptr)
        : QStyledItemDelegate(parent != nullptr ? parent : table), m_table(table),
          m_editorObjectName(std::move(editorObjectName)),
          m_choicesProvider(std::move(choicesProvider)),
          m_commitHandler(std::move(commitHandler))
    {
        Q_ASSERT(m_table != nullptr);
    }

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &,
                          const QModelIndex &) const override
    {
        auto *combo = new TableComboBox(parent);
        configureTableComboBox(combo, m_editorObjectName);
        combo->setProperty(kReadyProperty, false);
        combo->installEventFilter(const_cast<TableComboDelegate *>(this));
        auto *delegate = const_cast<TableComboDelegate *>(this);

        connect(combo, qOverload<int>(&QComboBox::activated), delegate,
                [delegate, combo](int) {
                    if (!combo->property(kReadyProperty).toBool()) {
                        return;
                    }

                    if (delegate->m_table == nullptr) {
                        return;
                    }

                    const int row = combo->property(kRowProperty).toInt();
                    const int column = combo->property(kColumnProperty).toInt();
                    if (row < 0 || row >= delegate->m_table->rowCount() || column < 0 ||
                        column >= delegate->m_table->columnCount()) {
                        return;
                    }

                    delegate->applyChoiceToModel(
                        combo, delegate->m_table->model(),
                        delegate->m_table->model()->index(row, column));
                });

        return combo;
    }

    bool eventFilter(QObject *watched, QEvent *event) override
    {
        auto *combo = qobject_cast<QComboBox *>(watched);
        if (combo != nullptr && m_table != nullptr && event->type() == QEvent::MouseButtonPress) {
            const int row = combo->property(kRowProperty).toInt();
            const int column = combo->property(kColumnProperty).toInt();
            if (row >= 0 && row < m_table->rowCount() && column >= 0 &&
                column < m_table->columnCount()) {
                if (auto *item = m_table->item(row, column); item != nullptr) {
                    m_table->setCurrentItem(item);
                    m_table->selectRow(row);
                }
            }
        }

        return QStyledItemDelegate::eventFilter(watched, event);
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const override
    {
        auto *combo = qobject_cast<QComboBox *>(editor);
        if (combo == nullptr) {
            return;
        }

        combo->setProperty(kReadyProperty, false);
        combo->clear();

        if (m_choicesProvider) {
            const QVector<TableComboChoice> choices = m_choicesProvider(index);
            for (const auto &choice : choices) {
                combo->addItem(choice.label, choice.value);
            }
        }

        const QVariant currentValue = index.data(Qt::UserRole);
        int currentIndex = currentValue.isValid() ? combo->findData(currentValue) : -1;
        if (currentIndex < 0) {
            const QString currentText = index.data(Qt::EditRole).toString();
            currentIndex = combo->findText(currentText);
            if (currentIndex < 0 && !currentText.isEmpty()) {
                combo->addItem(currentText, currentValue.isValid() ? currentValue : currentText);
                currentIndex = combo->count() - 1;
            }
        }

        if (currentIndex >= 0) {
            combo->setCurrentIndex(currentIndex);
        }

        combo->setProperty(kRowProperty, index.row());
        combo->setProperty(kColumnProperty, index.column());
        combo->setProperty(kReadyProperty, true);
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override
    {
        auto *combo = qobject_cast<QComboBox *>(editor);
        if (combo == nullptr || model == nullptr) {
            return;
        }

        applyChoiceToModel(combo, model, index);
    }

    void applyChoiceToModel(QComboBox *combo, QAbstractItemModel *model,
                            const QModelIndex &index) const
    {
        if (combo == nullptr || model == nullptr || !index.isValid()) {
            return;
        }

        const QString label = combo->currentText();
        const QVariant value = combo->currentData();
        const QVariant canonicalValue = value.isValid() ? value : QVariant(label);
        if (index.data(Qt::EditRole).toString() == label &&
            index.data(Qt::UserRole) == canonicalValue) {
            return;
        }
        model->setData(index, label, Qt::EditRole);
        model->setData(index, canonicalValue, Qt::UserRole);

        if (m_commitHandler) {
            m_commitHandler(index, TableComboChoice{.label = label, .value = canonicalValue});
        }
    }

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                              const QModelIndex &) const override
    {
        if (editor != nullptr) {
            const int horizontalInset = UiScale::scalePx(3);
            const int verticalInset = UiScale::scalePx(2);
            QRect editorRect =
                option.rect.adjusted(horizontalInset, verticalInset, -horizontalInset,
                                     -verticalInset);
            if (editorRect.width() < UiScale::scalePx(28) ||
                editorRect.height() < UiScale::scalePx(20)) {
                editorRect = option.rect.adjusted(1, 1, -1, -1);
            }
            editor->setGeometry(editorRect);
            editor->setMinimumHeight(editorRect.height());
            editor->setMaximumHeight(editorRect.height());
        }
    }

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override
    {
        QSize hint = QStyledItemDelegate::sizeHint(option, index);
        hint.setHeight(std::max(hint.height(), defaultInlineEditorRowHeight()));
        return hint;
    }

private:
    inline static constexpr auto kReadyProperty = "tableComboEditorReady";
    inline static constexpr auto kRowProperty = "tableEditorRow";
    inline static constexpr auto kColumnProperty = "tableEditorColumn";

    QPointer<QTableWidget> m_table;
    QString m_editorObjectName;
    ChoicesProvider m_choicesProvider;
    CommitHandler m_commitHandler;
};

inline void installComboDelegate(QTableWidget *table, int column, const QString &editorObjectName,
                                 TableComboDelegate::ChoicesProvider choicesProvider,
                                 TableComboDelegate::CommitHandler commitHandler = {})
{
    Q_ASSERT(table != nullptr);
    table->setItemDelegateForColumn(
        column, new TableComboDelegate(table, editorObjectName, std::move(choicesProvider),
                                       std::move(commitHandler), table));
    ensurePersistentComboSupport(table)->addColumn(column);
}

inline void configureTableViewportMetrics(QTableWidget *table)
{
    Q_ASSERT(table != nullptr);
    table->setWordWrap(false);
    table->setTextElideMode(Qt::ElideRight);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    table->verticalHeader()->setDefaultSectionSize(defaultInlineEditorRowHeight());
    table->verticalHeader()->setMinimumSectionSize(UiScale::scalePx(28));
    table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
}

inline void configureEditableTable(QTableWidget *table,
                                   const QString &editorObjectName = {})
{
    Q_ASSERT(table != nullptr);
    configureTableViewportMetrics(table);
    table->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    table->setItemDelegate(new TableLineEditDelegate(editorObjectName, {}, table));
}

inline void configureReadOnlyTable(QTableWidget *table)
{
    Q_ASSERT(table != nullptr);
    configureTableViewportMetrics(table);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

inline void installLineEditDelegateForColumn(
    QTableWidget *table, int column, const QString &editorObjectName = {},
    TableLineEditDelegate::Normalizer normalizer = {})
{
    Q_ASSERT(table != nullptr);
    table->setItemDelegateForColumn(
        column, new TableLineEditDelegate(editorObjectName, std::move(normalizer), table));
}

inline void installSpinBoxDelegate(QTableWidget *table, int column, int minimum, int maximum,
                                   const QString &editorObjectName = {})
{
    Q_ASSERT(table != nullptr);
    table->setItemDelegateForColumn(
        column, new TableSpinBoxDelegate(minimum, maximum, editorObjectName, table));
}

inline void installDoubleSpinBoxDelegate(QTableWidget *table, int column, double minimum,
                                         double maximum, int decimals, double singleStep,
                                         const QString &editorObjectName = {})
{
    Q_ASSERT(table != nullptr);
    table->setItemDelegateForColumn(
        column, new TableDoubleSpinBoxDelegate(minimum, maximum, decimals, singleStep,
                                               editorObjectName, table));
}

inline void installDateEditDelegate(QTableWidget *table, int column,
                                    const QString &format = QStringLiteral("yyyy-MM-dd"),
                                    const QString &editorObjectName = {})
{
    Q_ASSERT(table != nullptr);
    table->setItemDelegateForColumn(
        column, new TableDateEditDelegate(format, editorObjectName, table));
}

inline void syncTableRowHeight(QTableWidget *table, int row, int minimumHeight = 32)
{
    Q_ASSERT(table != nullptr);
    if (row < 0 || row >= table->rowCount()) {
        return;
    }

    int preferredHeight = std::max(minimumHeight, table->rowHeight(row));
    for (int column = 0; column < table->columnCount(); ++column) {
        if (auto *cellWidget = table->cellWidget(row, column); cellWidget != nullptr) {
            preferredHeight = std::max(preferredHeight, cellWidget->sizeHint().height() + 8);
        }
    }

    table->setRowHeight(row, preferredHeight);
}

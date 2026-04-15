#pragma once

#include <QAbstractItemView>
#include <QAbstractSpinBox>
#include <QApplication>
#include <QComboBox>
#include <QCoreApplication>
#include <QDate>
#include <QDateEdit>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QImage>
#include <QLineEdit>
#include <QListView>
#include <QPointer>
#include <QTableWidget>
#include <QWidget>
#include <QtTest>

namespace TableUiTestHelpers {
inline void waitForInlineEditSettle(int delayMs = 150)
{
    QCoreApplication::processEvents(QEventLoop::AllEvents, delayMs);
    QTest::qWait(delayMs);
}

inline bool hasTableEditorAncestor(const QWidget *widget)
{
    const QWidget *current = widget;
    while (current != nullptr) {
        if (current->property("tableEditor").toBool()) {
            return true;
        }
        current = current->parentWidget();
    }

    return false;
}

inline QWidget *findEditorContainer(QWidget *widget)
{
    QWidget *current = widget;
    while (current != nullptr) {
        if (current->property("tableEditor").toBool()) {
            return current;
        }
        current = current->parentWidget();
    }

    return nullptr;
}

inline QWidget *findEditorWidget(QTableWidget *table)
{
    if (table == nullptr) {
        return nullptr;
    }

    if (auto *focusedWidget = QApplication::focusWidget()) {
        if (auto *container = findEditorContainer(focusedWidget); container != nullptr) {
            if (!container->property("tablePersistentEditor").toBool()) {
                return container;
            }
        }
    }

    const auto widgets = table->findChildren<QWidget *>();
    for (auto *widget : widgets) {
        if (widget->isVisible() && widget->property("tableEditor").toBool() &&
            !widget->property("tablePersistentEditor").toBool()) {
            return widget;
        }
    }

    return nullptr;
}

inline QComboBox *findPersistentComboEditor(QTableWidget *table, int row, int column)
{
    if (table == nullptr || row < 0 || column < 0) {
        return nullptr;
    }

    auto *item = table->item(row, column);
    if (item == nullptr) {
        return nullptr;
    }

    const QRect cellRect = table->visualItemRect(item);
    const QPoint cellCenter = cellRect.center();

    const auto combos = table->findChildren<QComboBox *>();
    for (auto *combo : combos) {
        if (!combo->isVisible() || !combo->property("tablePersistentEditor").toBool()) {
            continue;
        }

        const QPoint topLeft = combo->mapTo(table->viewport(), QPoint(0, 0));
        const QRect comboRect(topLeft, combo->size());
        if (!comboRect.contains(cellCenter)) {
            continue;
        }

        const bool rowMatches = combo->property("tableEditorRow").toInt() == row;
        const bool columnMatches = combo->property("tableEditorColumn").toInt() == column;
        if ((rowMatches && columnMatches) || !combo->property("tableEditorRow").isValid()) {
            return combo;
        }
    }

    return nullptr;
}

inline QLineEdit *findLineEditEditor(QTableWidget *table)
{
    if (table == nullptr) {
        return nullptr;
    }

    if (auto *focusedLineEdit = qobject_cast<QLineEdit *>(QApplication::focusWidget());
        focusedLineEdit != nullptr && hasTableEditorAncestor(focusedLineEdit)) {
        return focusedLineEdit;
    }

    const auto lineEdits = table->findChildren<QLineEdit *>();
    for (auto *lineEdit : lineEdits) {
        if (lineEdit->isVisible() && hasTableEditorAncestor(lineEdit)) {
            return lineEdit;
        }
    }

    return nullptr;
}

inline QWidget *activateEditor(QTableWidget *table, int row, int column)
{
    Q_ASSERT(table != nullptr);
    Q_ASSERT(table->item(row, column) != nullptr);
    if (auto *persistentCombo = findPersistentComboEditor(table, row, column);
        persistentCombo != nullptr) {
        table->setCurrentItem(table->item(row, column));
        table->selectRow(row);
        persistentCombo->setFocus(Qt::OtherFocusReason);
        return persistentCombo;
    }

    QPointer<QWidget> previousEditor = findEditorWidget(table);
    if (previousEditor != nullptr) {
        if (auto *lineEdit = qobject_cast<QLineEdit *>(previousEditor.data()); lineEdit != nullptr) {
            QTest::keyClick(lineEdit, Qt::Key_Enter);
        } else if (auto *comboBox = qobject_cast<QComboBox *>(previousEditor.data());
                   comboBox != nullptr) {
            QTest::keyClick(comboBox, Qt::Key_Enter);
        } else {
            previousEditor->clearFocus();
        }
        QElapsedTimer closeTimer;
        closeTimer.start();
        while (closeTimer.elapsed() < 1000) {
            if (previousEditor == nullptr || !previousEditor->isVisible()) {
                break;
            }

            QCoreApplication::processEvents(QEventLoop::AllEvents, 25);
            QTest::qWait(10);
        }
    }

    if (auto *item = table->item(row, column); item != nullptr) {
        table->scrollToItem(item);
        table->setCurrentItem(item);
    }
    table->setFocus(Qt::OtherFocusReason);
    QCoreApplication::processEvents(QEventLoop::AllEvents, 25);

    const auto *editableItem = table->item(row, column);
    if (editableItem == nullptr) {
        return nullptr;
    }
    const QRect itemRect = table->visualItemRect(editableItem);
    QTest::mouseDClick(table->viewport(), Qt::LeftButton, Qt::NoModifier, itemRect.center());

    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < 1000) {
        if (auto *editor = findEditorWidget(table);
            editor != nullptr && editor != previousEditor.data()) {
            return editor;
        }

        QCoreApplication::processEvents(QEventLoop::AllEvents, 25);
        QTest::qWait(10);
    }

    if (findEditorWidget(table) == nullptr) {
        if (auto *item = table->item(row, column); item != nullptr) {
            table->editItem(item);
        }
        if (findEditorWidget(table) == nullptr) {
            table->edit(table->model()->index(row, column));
        }
    }

    timer.restart();
    while (timer.elapsed() < 1000) {
        if (auto *editor = findEditorWidget(table);
            editor != nullptr && editor != previousEditor.data()) {
            return editor;
        }

        QCoreApplication::processEvents(QEventLoop::AllEvents, 25);
        QTest::qWait(10);
    }

    return findEditorWidget(table);
}

inline void editTextCell(QTableWidget *table, int row, int column, const QString &text)
{
    QWidget *editor = activateEditor(table, row, column);
    QTRY_VERIFY_WITH_TIMEOUT(editor != nullptr, 500);

    if (auto *dateEdit = qobject_cast<QDateEdit *>(editor); dateEdit != nullptr) {
        const QDate date = QDate::fromString(text, dateEdit->displayFormat());
        if (date.isValid()) {
            dateEdit->setDate(date);
            QTest::keyClick(dateEdit, Qt::Key_Enter);
            QTRY_VERIFY_WITH_TIMEOUT(findEditorWidget(table) == nullptr, 1000);
            waitForInlineEditSettle();
            return;
        }
    }

    QWidget *inputTarget = findLineEditEditor(table);
    if (inputTarget == nullptr) {
        inputTarget = editor;
    }
    if (inputTarget == nullptr) {
        qFatal("Unable to locate an inline editor for the requested table cell.");
    }

    if (auto *lineEdit = qobject_cast<QLineEdit *>(inputTarget); lineEdit != nullptr) {
        lineEdit->selectAll();
    } else if (auto *spinBox = qobject_cast<QAbstractSpinBox *>(editor); spinBox != nullptr) {
        spinBox->selectAll();
    } else {
        inputTarget->setFocus(Qt::OtherFocusReason);
        QTest::keyClick(inputTarget, Qt::Key_A, Qt::ControlModifier);
    }

    QTest::keyClicks(inputTarget, text);
    QTest::keyClick(inputTarget, Qt::Key_Enter);
    QTRY_VERIFY_WITH_TIMEOUT(findEditorWidget(table) == nullptr, 1000);
    waitForInlineEditSettle();
}

inline QComboBox *comboCell(QTableWidget *table, int row, int column)
{
    Q_ASSERT(table != nullptr);
    if (auto *persistentCombo = findPersistentComboEditor(table, row, column);
        persistentCombo != nullptr) {
        table->setCurrentItem(table->item(row, column));
        table->selectRow(row);
        return persistentCombo;
    }

    activateEditor(table, row, column);

    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < 1000) {
        if (auto *focusedCombo = qobject_cast<QComboBox *>(QApplication::focusWidget());
            focusedCombo != nullptr && focusedCombo->property("tableEditor").toBool()) {
            return focusedCombo;
        }

        const auto combos = table->findChildren<QComboBox *>();
        for (auto *combo : combos) {
            if (combo->isVisible() && combo->property("tableEditor").toBool()) {
                return combo;
            }
        }

        QCoreApplication::processEvents(QEventLoop::AllEvents, 25);
        QTest::qWait(10);
    }

    qFatal("Unable to locate a combo box editor for the requested table cell.");
    return nullptr;
}

inline QListView *comboPopupView(QComboBox *combo)
{
    Q_ASSERT(combo != nullptr);
    return qobject_cast<QListView *>(combo->view());
}

inline QWidget *comboPopupContainer(QComboBox *combo)
{
    auto *popupView = comboPopupView(combo);
    return popupView != nullptr ? popupView->window() : nullptr;
}

inline QList<QWidget *> visibleComboPopupContainers()
{
    QList<QWidget *> popups;
    for (QWidget *topLevel : QApplication::topLevelWidgets()) {
        if (topLevel != nullptr && topLevel->isVisible() &&
            topLevel->property("tableComboPopupContainer").toBool() && topLevel->width() > 8 &&
            topLevel->height() > 8) {
            popups.append(topLevel);
        }
    }

    return popups;
}

inline QWidget *showComboPopup(QComboBox *combo)
{
    Q_ASSERT(combo != nullptr);
    combo->setFocus(Qt::OtherFocusReason);
    combo->showPopup();

    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < 1000) {
        auto *popupView = comboPopupView(combo);
        auto *popupContainer = comboPopupContainer(combo);
        if (popupView != nullptr && popupView->isVisible() && popupContainer != nullptr &&
            popupContainer->isVisible() && popupContainer->width() > 8 &&
            popupContainer->height() > 8) {
            return popupContainer;
        }

        QCoreApplication::processEvents(QEventLoop::AllEvents, 25);
        QTest::qWait(10);
    }

    qFatal("Unable to show a visible combo popup for the requested editor.");
    return nullptr;
}

inline void hideComboPopup(QComboBox *combo)
{
    Q_ASSERT(combo != nullptr);
    combo->hidePopup();

    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < 1000) {
        auto *popupView = comboPopupView(combo);
        auto *popupContainer = comboPopupContainer(combo);
        const bool popupHidden = (popupView == nullptr || !popupView->isVisible()) &&
                                 (popupContainer == nullptr || !popupContainer->isVisible());
        if (popupHidden) {
            return;
        }

        QCoreApplication::processEvents(QEventLoop::AllEvents, 25);
        QTest::qWait(10);
    }

    qFatal("Combo popup did not close in time.");
}

inline QImage grabWidgetImage(QWidget *widget)
{
    Q_ASSERT(widget != nullptr);
    return widget->grab().toImage();
}

inline double darkPixelRatio(const QImage &image)
{
    if (image.isNull() || image.width() <= 0 || image.height() <= 0) {
        return 1.0;
    }

    int sampled = 0;
    int dark = 0;
    const int stepX = std::max(1, image.width() / 64);
    const int stepY = std::max(1, image.height() / 64);

    for (int y = 0; y < image.height(); y += stepY) {
        for (int x = 0; x < image.width(); x += stepX) {
            const QRgb pixel = image.pixel(x, y);
            if (qAlpha(pixel) < 220) {
                continue;
            }

            ++sampled;
            if (qGray(pixel) < 48) {
                ++dark;
            }
        }
    }

    return sampled > 0 ? static_cast<double>(dark) / static_cast<double>(sampled) : 1.0;
}

inline void setComboCellByText(QTableWidget *table, int row, int column, const QString &text)
{
    auto *combo = comboCell(table, row, column);
    const bool persistent = combo->property("tablePersistentEditor").toBool();
    const int index = combo->findText(text);
    Q_ASSERT(index >= 0);
    if (persistent) {
        combo->setFocus(Qt::OtherFocusReason);
        combo->setCurrentIndex(index);
        QMetaObject::invokeMethod(combo, "activated", Qt::DirectConnection, Q_ARG(int, index));
        waitForInlineEditSettle();
        return;
    }

    combo->showPopup();
    QTRY_VERIFY(combo->view() != nullptr);
    combo->view()->setCurrentIndex(combo->model()->index(index, combo->modelColumn()));
    QTRY_VERIFY(combo->view()->currentIndex().isValid());
    const QRect itemRect = combo->view()->visualRect(combo->view()->currentIndex());
    QTest::mouseClick(combo->view()->viewport(), Qt::LeftButton, Qt::NoModifier,
                      itemRect.center());
    if (!persistent) {
        QTRY_VERIFY_WITH_TIMEOUT(findEditorWidget(table) == nullptr, 1000);
    }
    waitForInlineEditSettle();
}
} // namespace TableUiTestHelpers

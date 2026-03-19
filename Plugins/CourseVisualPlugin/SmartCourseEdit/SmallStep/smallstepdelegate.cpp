#include "smallstepdelegate.h"
#include "smallstepmodel.h"
#include <QPainter>
#include <QLineEdit>
#include <QStyle>

SmallStepDelegate::SmallStepDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
    , m_model(nullptr)
    , m_isEditing(false)
{
}

SmallStepDelegate::~SmallStepDelegate()
{
}

void SmallStepDelegate::setModel(SmallStepModel *model)
{
    m_model = model;
}

void SmallStepDelegate::setEditingIndex(const QModelIndex &index)
{
    m_editingIndex = index;
}

bool SmallStepDelegate::isIndexEditing(const QModelIndex &index) const
{
    return m_editingIndex == index;
}

void SmallStepDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    int row = index.row();
    bool isAddItem = m_model && m_model->isAddItem(row);
    bool isSelected = !isAddItem && m_model && (row == m_model->selectedIndex());

    if (!isIndexEditing(index)) {
        if (isAddItem) {
            QRect itemRect = option.rect.adjusted(5, 5, -5, -5);
            drawAddItem(painter, itemRect);
        } else {
            QRect itemRect = option.rect.adjusted(5, 5, -5, -5);
            QString name = index.data(Qt::DisplayRole).toString();
            drawStepItem(painter, itemRect, name, isSelected);
        }
    }

    painter->restore();
}

QSize SmallStepDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    return QSize(200, 50);
}

QWidget *SmallStepDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    m_isEditing = true;
    m_editingIndex = index;

    QLineEdit *editor = new QLineEdit(parent);
    editor->setFrame(false);
    editor->setStyleSheet("border: none; background: transparent;");
    editor->setTextMargins(15, 0, 10, 0);
    return editor;
}

void SmallStepDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    if (m_model && m_model->isAddItem(index.row())) {
        lineEdit->setText(QString());
    } else {
        lineEdit->setText(index.data().toString());
        lineEdit->selectAll();
    }
}

void SmallStepDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    model->setData(index, lineEdit->text(), Qt::EditRole);
    m_isEditing = false;
    m_editingIndex = QModelIndex();
}

void SmallStepDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}

void SmallStepDelegate::drawStepItem(QPainter *painter, const QRect &rect, const QString &name, bool isSelected) const
{
    QColor bgColor = isSelected ? QColor(220, 240, 255) : QColor(250, 250, 250);
    QColor borderColor = isSelected ? QColor(100, 160, 255) : QColor(200, 200, 200);

    painter->setPen(QPen(borderColor, isSelected ? 2 : 1));
    painter->setBrush(bgColor);
    painter->drawRoundedRect(rect, 6, 6);

    painter->setPen(isSelected ? QColor(50, 100, 200) : Qt::black);
    painter->setFont(QFont("Microsoft YaHei", 9));
    painter->drawText(rect.adjusted(15, 0, -10, 0), Qt::AlignLeft | Qt::AlignVCenter, name);
}

void SmallStepDelegate::drawAddItem(QPainter *painter, const QRect &rect) const
{
    QColor bgColor = QColor(240, 245, 250);
    QColor borderColor = QColor(190, 200, 215);

    painter->setPen(QPen(borderColor, 1));
    painter->setBrush(bgColor);
    painter->drawRoundedRect(rect, 6, 6);

    QFont font = QFont("Microsoft YaHei", 10);
    font.setBold(true);
    painter->setFont(font);

    painter->setPen(QColor(100, 150, 200));

    QFontMetrics fm(font);
    QString text = QString::fromLocal8Bit("+ 点击添加步骤");
    int textWidth = fm.horizontalAdvance(text);
    int textX = rect.left() + (rect.width() - textWidth) / 2;

    painter->drawText(textX, rect.center().ry() + fm.ascent() / 2, text);
}

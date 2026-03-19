#include "largestepdelegate.h"
#include "largestepmodel.h"
#include <QPainter>
#include <QLineEdit>
#include <QStyle>
#include <QPainterPath>

LargeStepDelegate::LargeStepDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
    , m_model(nullptr)
    , m_isEditing(false)
{
}

LargeStepDelegate::~LargeStepDelegate()
{
}

void LargeStepDelegate::setModel(LargeStepModel *model)
{
    m_model = model;
}

void LargeStepDelegate::setEditingIndex(const QModelIndex &index)
{
    m_editingIndex = index;
}

bool LargeStepDelegate::isIndexEditing(const QModelIndex &index) const
{
    return m_editingIndex == index;
}

void LargeStepDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
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
            int stepCount = m_model ? m_model->rowCount() - 1 : index.model()->rowCount() - 1;
            QRect itemRect = option.rect.adjusted(5, 5, -25, -5);
            QString name = index.data(Qt::DisplayRole).toString();
            drawStepItem(painter, itemRect, name, isSelected);

            if (row < stepCount - 1) {
                QRect arrowRect = option.rect.adjusted(180, 5, -5, -5);
                drawArrow(painter, arrowRect);
            }
        }
    }

    painter->restore();
}

QSize LargeStepDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    return QSize(200, 50);
}

QWidget *LargeStepDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
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

void LargeStepDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    if (m_model && m_model->isAddItem(index.row())) {
        lineEdit->setText(QString());
    } else {
        lineEdit->setText(index.data().toString());
        lineEdit->selectAll();
    }
}

void LargeStepDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    model->setData(index, lineEdit->text(), Qt::EditRole);
    m_isEditing = false;
    m_editingIndex = QModelIndex();
}

void LargeStepDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect);
}

void LargeStepDelegate::drawStepItem(QPainter *painter, const QRect &rect, const QString &name, bool isSelected) const
{
    QColor bgColor = isSelected ? QColor(220, 240, 255) : QColor(245, 245, 245);
    QColor borderColor = isSelected ? QColor(100, 160, 255) : QColor(200, 200, 200);

    painter->setPen(QPen(borderColor, isSelected ? 2 : 1));
    painter->setBrush(bgColor);
    painter->drawRoundedRect(rect, 8, 8);

    painter->setPen(isSelected ? QColor(50, 100, 200) : Qt::black);
    painter->setFont(QFont("Microsoft YaHei", 10));
    painter->drawText(rect.adjusted(15, 0, -10, 0), Qt::AlignLeft | Qt::AlignVCenter, name);
}

void LargeStepDelegate::drawArrow(QPainter *painter, const QRect &rect) const
{
    int centerY = rect.top() + rect.height() / 2;

    painter->setPen(QPen(Qt::gray, 2));
    painter->drawLine(rect.left(), centerY, rect.right() - 5, centerY);

    QPainterPath path;
    path.moveTo(rect.right(), centerY);
    path.lineTo(rect.right() - 6, centerY - 6);
    path.lineTo(rect.right() - 6, centerY + 6);
    path.closeSubpath();
    painter->setBrush(Qt::gray);
    painter->drawPath(path);
}

void LargeStepDelegate::drawAddItem(QPainter *painter, const QRect &rect) const
{
    QColor bgColor = QColor(230, 240, 250);
    QColor borderColor = QColor(180, 200, 220);

    painter->setPen(QPen(borderColor, 1));
    painter->setBrush(bgColor);
    painter->drawRoundedRect(rect, 8, 8);

    QFont font = QFont("Microsoft YaHei", 12);
    font.setBold(true);
    painter->setFont(font);

    painter->setPen(QColor(100, 150, 200));

    QFontMetrics fm(font);
    QString text = QString::fromLocal8Bit("+ 点击添加步骤");
    int textWidth = fm.horizontalAdvance(text);
    int textX = rect.left() + (rect.width() - textWidth) / 2;

    painter->drawText(textX, rect.center().ry() + fm.ascent() / 2, text);
}

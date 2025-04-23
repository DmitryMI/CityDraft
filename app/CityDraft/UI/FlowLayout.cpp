//
// Created by mariiakorolevaa on 19.04.2025.
//

#include "FlowLayout.h"
#include "ImageSelectionWidget.h"

FlowLayout::FlowLayout(QWidget *parent, const int margin, const int hSpacing, const int vSpacing)
    : QLayout(parent), m_hSpace(hSpacing), m_vSpace(vSpacing)
{
    setContentsMargins(margin, margin, margin, margin);
}

FlowLayout::~FlowLayout()
{
    QLayoutItem* item;
    while ((item = FlowLayout::takeAt(0)))
    {
        delete item;
    }
}

void FlowLayout::addItem(QLayoutItem* item)
{
    itemList.append(item);
}

QLayoutItem* FlowLayout::takeAt(int index)
{
    if (index >= 0 && index < itemList.size())
        return itemList.takeAt(index);
    return nullptr;
}

QLayoutItem* FlowLayout::itemAt(int index) const
{
    return (index >= 0 && index < itemList.size()) ? itemList.at(index) : nullptr;
}

int FlowLayout::count() const
{
    return itemList.size();
}

int FlowLayout::horizontalSpacing() const
{
    return m_hSpace >= 0 ? m_hSpace : smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
}

int FlowLayout::verticalSpacing() const
{
    return m_vSpace >= 0 ? m_vSpace : smartSpacing(QStyle::PM_LayoutVerticalSpacing);
}

Qt::Orientations FlowLayout::expandingDirections() const
{
    return {};
}

bool FlowLayout::hasHeightForWidth() const
{
    return true;
}

int FlowLayout::heightForWidth(const int width) const
{
    return doLayout(QRect(0, 0, width, 0), true);  // testOnly == true
}

QSize FlowLayout::minimumSize() const
{
    QSize size;
    for (const QLayoutItem* item : itemList)
    {
        size = size.expandedTo(item->minimumSize());
    }
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    return size + QSize(left + right, top + bottom);
}

void FlowLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    doLayout(rect, false);
}

QSize FlowLayout::sizeHint() const
{
    return minimumSize();
}

int FlowLayout::doLayout(const QRect &rect, bool testOnly) const
{
    int x = rect.x(), y = rect.y();
    int lineHeight = 0;

    for (QLayoutItem* item : itemList) {
        const int spaceX = horizontalSpacing();
        const int spaceY = verticalSpacing();

        int itemWidth = item->sizeHint().width();
        int itemHeight = item->sizeHint().height();

        if (x + itemWidth + spaceX > rect.right()) {
            x = rect.x();
            y += lineHeight + spaceY;
            lineHeight = 0;
        }

        if (!testOnly) {
            item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));
        }

        x += itemWidth + spaceX;

        lineHeight = std::max(lineHeight, itemHeight);
    }

    return y + lineHeight - rect.y();
}

int FlowLayout::smartSpacing(QStyle::PixelMetric pm) const
{
    QObject* parent = this->parent();
    if (!parent) return -1;

    if (parent->isWidgetType()) {
        const auto pw = dynamic_cast<QWidget *>(parent);
        return pw->style()->pixelMetric(pm, nullptr, pw);
    }

    return dynamic_cast<QLayout *>(parent)->spacing();
}

void FlowLayout::addLayout(QLayout* layout)
{
    if (layout) {
        itemList.append(layout);
    }
}
//
// Created by mariiakorolevaa on 19.04.2025.
//

#pragma once

#include <qlayout.h>
#include <QStyle>

class FlowLayout final : public QLayout
{
    public:
    explicit FlowLayout(QWidget *parent = nullptr, int margin = -1, int hSpacing = -1, int vSpacing = -1);
    ~FlowLayout() override;

    void addItem(QLayoutItem* item) override;
    int horizontalSpacing() const;
    int verticalSpacing() const;
    Qt::Orientations expandingDirections() const override;
    bool hasHeightForWidth() const override;
    int heightForWidth(int width) const override;
    int count() const override;
    QLayoutItem* itemAt(int index) const override;
    QSize minimumSize() const override;
    void setGeometry(const QRect& rect) override;
    QSize sizeHint() const override;
    QLayoutItem* takeAt(int index) override;
    int smartSpacing(QStyle::PixelMetric pm) const;
    void addLayout(QLayout* layout);
    int doLayout(const QRect &rect, bool expanding) const;

    private:
    int m_hSpace;
    int m_vSpace;
    QList<QLayoutItem*> itemList;
};

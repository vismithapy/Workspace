#ifndef CUSTOMITEM_H
#define CUSTOMITEM_H

#include <QGraphicsPixmapItem>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QPixmap>
#include <QString>
#include <QInputDialog>
#include <QMenu>

class CustomItem : public QGraphicsPixmapItem
{
public:
    void setShapeColor(const QColor &color) { shapeColor = color; }
    QColor getShapeColor() const { return shapeColor; }
    enum class ShapeType { Rectangle, Circle, Triangle, Unknown };

    explicit CustomItem(const QString &name,
                        const QString &imagePath,
                        QGraphicsItem *parent = nullptr);

    QString getName() const { return itemName; }
    void setShapeType(ShapeType type) { shapeType = type; }

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override; // Right-click menu
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;        // Start resizing
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;         // Handle resizing
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;      // Stop resizing
    void keyPressEvent(QKeyEvent *event) override;                         // Handle keyboard events for resizing

private:
    void resizeItem(qreal scaleFactor);        // Resize the item by a factor
    void resizeShape(qreal newWidth, qreal newHeight); // Resize shape based on type

    QString itemName;
    QPixmap originalPixmap; // Store the original pixmap to maintain clarity during resizing
    bool resizing = false;  // Flag to check if resizing is active
    QPointF lastMousePos;   // Last position of the mouse during resizing
    QRectF originalRect;    // Store the original size for scaling

    ShapeType shapeType = ShapeType::Unknown; // Identify the shape type (Rectangle, Circle, Triangle)
    QColor shapeColor = QColor("#4CAF50");
};

#endif // CUSTOMITEM_H

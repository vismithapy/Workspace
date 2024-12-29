#include "customitem.h"
#include <QAction>
#include <QCursor>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QMenu>
#include <QPainter>
#include <QInputDialog>

CustomItem::CustomItem(const QString &name, const QString &imagePath, QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
    , itemName(name)
    , originalPixmap(imagePath)
{
    // Load and scale the image
    setPixmap(originalPixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Set flags for movability and selectability
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable
             | QGraphicsItem::ItemIsFocusable);

    // Set the transform origin point to the center
    setTransformOriginPoint(boundingRect().center());
}

// Right-click context menu with multiple options
void CustomItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;

    // Add rotation options
    QAction *rotate45 = menu.addAction("Rotate 45°");
    QAction *rotate90 = menu.addAction("Rotate 90°");
    QAction *rotate180 = menu.addAction("Rotate 180°");
    QAction *rotateMinus45 = menu.addAction("Rotate -45°");
    QAction *resetRotation = menu.addAction("Reset Rotation");

    // Add resize and delete options
    QAction *resizeAction = menu.addAction("Manual Resize");
    QAction *deleteAction = menu.addAction("Delete");

    // Execute menu and capture selected action
    QAction *selectedAction = menu.exec(event->screenPos());

    if (selectedAction == rotate45) {
        setRotation(rotation() + 45); // Rotate item by 45 degrees clockwise
    } else if (selectedAction == rotate90) {
        setRotation(rotation() + 90); // Rotate item by 90 degrees clockwise
    } else if (selectedAction == rotate180) {
        setRotation(rotation() + 180); // Rotate item by 180 degrees
    } else if (selectedAction == rotateMinus45) {
        setRotation(rotation() - 45); // Rotate item by 45 degrees counterclockwise
    } else if (selectedAction == resetRotation) {
        setRotation(0); // Reset rotation to 0 degrees
    } else if (selectedAction == resizeAction) {
        bool ok;
        int width = QInputDialog::getInt(nullptr, "Resize Width", "Enter new width:", pixmap().width(), 20, 1000, 1, &ok);
        int height = QInputDialog::getInt(nullptr, "Resize Height", "Enter new height:", pixmap().height(), 20, 1000, 1, &ok);
        if (ok) {
            resizeShape(width, height);
        }
    } else if (selectedAction == deleteAction) {
        if (scene()) {
            scene()->removeItem(this);
            delete this;
        }
    }
}

// Mouse press event for resizing
void CustomItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        QRectF bounds = boundingRect();
        QPointF clickPos = event->pos();

        // Detect if the user clicked near the bottom-right corner
        if (qAbs(clickPos.x() - bounds.width()) < 10 && qAbs(clickPos.y() - bounds.height()) < 10) {
            resizing = true;
            setCursor(Qt::SizeFDiagCursor);
            lastMousePos = event->scenePos();
            originalRect = boundingRect(); // Store the original size
        } else {
            QGraphicsPixmapItem::mousePressEvent(event);
        }
    }
}

// Mouse move event for resizing
void CustomItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (resizing) {
        QPointF delta = event->scenePos() - lastMousePos;

        // Calculate the new size
        qreal newWidth = originalRect.width() + delta.x();
        qreal newHeight = originalRect.height() + delta.y();

        // Enforce minimum size constraints
        if (newWidth > 20 && newHeight > 20) {
            resizeShape(newWidth, newHeight);
        }
    } else {
        QGraphicsPixmapItem::mouseMoveEvent(event);
    }
}

// Mouse release event
void CustomItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (resizing) {
        resizing = false; // Exit resize mode
        setCursor(Qt::ArrowCursor);
    }
    QGraphicsPixmapItem::mouseReleaseEvent(event);
}

// Handle key press events for resizing
void CustomItem::keyPressEvent(QKeyEvent *event)
{
    if (!isSelected())
        return;

    if (event->modifiers() & Qt::ControlModifier) {
        if (event->key() == Qt::Key_Plus || event->key() == Qt::Key_Equal) {
            resizeItem(1.1); // Increase size by 10%
        } else if (event->key() == Qt::Key_Minus) {
            resizeItem(0.9); // Decrease size by 10%
        }
    }
}

void CustomItem::resizeItem(qreal scaleFactor)
{
    qreal newWidth = pixmap().width() * scaleFactor;
    qreal newHeight = pixmap().height() * scaleFactor;

    // Enforce minimum size constraints
    if (newWidth < 20 || newHeight < 20) {
        return;
    }

    QPixmap newPixmap(newWidth, newHeight);
    newPixmap.fill(Qt::transparent);
    QPainter painter(&newPixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::yellow);

    if (shapeType == ShapeType::Unknown) {
        // Handle image-based objects (e.g., Chair)
        setPixmap(originalPixmap.scaled(newWidth,
                                        newHeight,
                                        Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation));
    } else {
        // Handle Shape-based objects
        switch (shapeType) {
        case ShapeType::Rectangle:
            painter.drawRect(0, 0, newWidth, newHeight);
            break;

        case ShapeType::Circle:
            painter.drawEllipse(0, 0, newWidth, newHeight);
            break;

        case ShapeType::Triangle: {
            QPolygonF triangle;
            triangle << QPointF(newWidth / 2, 0)
                     << QPointF(newWidth, newHeight)
                     << QPointF(0, newHeight);
            painter.drawPolygon(triangle);
            break;
        }

        default:
            break;
        }
        painter.end();
        setPixmap(newPixmap);
    }

    update(); // Redraw the item in the scene
}

void CustomItem::resizeShape(qreal newWidth, qreal newHeight)
{
    qreal minWidth = 20;
    qreal minHeight = 20;

    if (newWidth < minWidth || newHeight < minHeight) {
        return; // Prevent shapes from becoming too small
    }

    QPixmap newPixmap(newWidth, newHeight);
    newPixmap.fill(Qt::transparent);
    QPainter painter(&newPixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(shapeColor); // Use the current shape color

    if (shapeType == ShapeType::Unknown) {
        // Handle image-based objects (e.g., Chair)
        setPixmap(originalPixmap.scaled(newWidth,
                                        newHeight,
                                        Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation));
    } else {
        // Handle Shape-based objects
        switch (shapeType) {
        case ShapeType::Rectangle:
            painter.drawRect(0, 0, newWidth, newHeight);
            break;

        case ShapeType::Circle:
            painter.drawEllipse(0, 0, newWidth, newHeight);
            break;

        case ShapeType::Triangle: {
            QPolygonF triangle;
            triangle << QPointF(newWidth / 2, 0)
                     << QPointF(newWidth, newHeight)
                     << QPointF(0, newHeight);
            painter.drawPolygon(triangle);
            break;
        }

        default:
            break;
        }
    }

    painter.end();
    setPixmap(newPixmap);
    update(); // Ensure redraw on the scene
}




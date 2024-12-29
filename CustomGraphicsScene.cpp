#include "customgraphicsscene.h"
#include <QGraphicsItem>
#include <QGraphicsSceneContextMenuEvent>
#include <QInputDialog>
#include <QMenu>

CustomGraphicsScene::CustomGraphicsScene(QObject *parent)
    : QGraphicsScene(parent)
{}

void CustomGraphicsScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QGraphicsItem *item = itemAt(event->scenePos(), QTransform());
    if (item) {
        QMenu menu;
        QAction *deleteAction = menu.addAction("Delete");
        QAction *resizeAction = menu.addAction("Resize");

        QAction *selectedAction = menu.exec(event->screenPos());
        if (selectedAction == deleteAction) {
            emit shapeDeleted(item);
        } else if (selectedAction == resizeAction) {
            emit shapeResized(item);
        }
    }
}

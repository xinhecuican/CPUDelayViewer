#ifndef GRAPHPANEL_H_
#define GRAPHPANEL_H_
#include <QWidget>
#include "graphviewport.h"
#include "../DataLoader/dataloader.h"
#include "graphscene.h"
#include "graphtoolbar.h"
#include "graphdialog.h"

class GraphPanel : public QWidget {
    Q_OBJECT
public:
    GraphPanel(GraphDialog* dialog, DataLoader* loader, QWidget* parent = nullptr);
    void setQuery(const QString& query, quint64 start, quint64 end);

private:
    GraphDialog* dialog;
    DataLoader* loader;
    GraphToolbar* toolbar;
    GraphViewport* viewport;
    GraphScene* scene;
    GraphDialog::GraphDialogInfo info;
};

#endif

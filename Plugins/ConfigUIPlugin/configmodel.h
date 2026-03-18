#ifndef CONFIGMODEL_H
#define CONFIGMODEL_H

#include <QStandardItemModel>
#include <QMimeData>
#include <QDataStream>

class ConfigModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit ConfigModel(QObject *parent = nullptr);

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    Qt::DropActions supportedDropActions() const override;

    void setConfigManager(void *configManager);
    void loadConfig();
    void saveConfig();

private:
    void *m_configManager;
};

#endif // CONFIGMODEL_H

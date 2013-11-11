#ifndef DFXML_ABSTRACTLISTMODELCHECKABLE_H
#define DFXML_ABSTRACTLISTMODELCHECKABLE_H

#include <QXmlStreamReader>
#include <QAbstractListModel>
#include <QVector>

#include "fileobject.h"
#include "source.h"

class DfxmlParser;

class DfxmlModelItem
{
	public:
		DfxmlModelItem(const DfxmlFileObject &obj = DfxmlFileObject(),
		     const Qt::CheckState checked = Qt::Checked)
			: m_object(obj),
			  m_checked(checked)
		{
		}

		DfxmlFileObject m_object;
		Qt::CheckState  m_checked;
};

class DfxmlListModel : public QAbstractListModel
{
	public:
		DfxmlListModel();

		QVariant data(const QModelIndex& index, int role) const;
		bool setData (const QModelIndex &index, const QVariant &value, int role);
		int rowCount(const QModelIndex  &parent = QModelIndex()) const;
		Qt::ItemFlags flags(const QModelIndex &index) const;

		bool setXmlFile(const QString &xmlFile);
		bool saveXmlReport(const QString &fileName);

		DfxmlFileObject fileObject(const QModelIndex &index);
		quint64    fileObjectSize(const QModelIndex &index);
		QByteArray fileObjectData(const QModelIndex &index);
		QString    fileObjectName(const QModelIndex &index);
		bool       fileObjectChecked(const QModelIndex &index);

		QString    sourceFileName() const;

		DfxmlSource *source();

		void analyze() const;

		bool fileObjectSave(const QModelIndex &index,
				    const QString &filePath,
				    QString &errorString) const;

	private:
		QList<DfxmlModelItem> m_items;

		DfxmlSource m_source;
		DfxmlParser *m_parser;
};

#endif

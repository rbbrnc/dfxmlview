#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTemporaryFile>
#include <QSortFilterProxyModel>
#include <QItemSelectionModel>
#include "model.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

	private slots:
	        void onListViewClicked(const QModelIndex &index);
		void onPreviewToggled(bool checked);

		void onFileObjectChanged(const QModelIndex &current, const QModelIndex &previous);

	        void onDfxmlLoad();
	        void onDfxmlSave();

	        void onObjectSave();
	        void onObjectInfo();
	        void onObjectView();
		void onObjectViewFilter();

		void showContextMenu(const QPoint &pos);
	        void onSourceInfo();
	        void onSaveSectorMap();

		void onSplitterMoved(int pos, int index);
		void updatePreview(const QModelIndex &index);

	private:
		bool checkSourceFile();

		Ui::MainWindow *ui;

		DfxmlListModel *m_model;
		QSortFilterProxyModel *m_proxyModel;
		QString m_currentFile;

		QList<QTemporaryFile *> m_tmpFiles;
		QItemSelectionModel *m_selectionModel;
};

#endif // MAINWINDOW_H

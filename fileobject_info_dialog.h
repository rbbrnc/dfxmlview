#ifndef FILEOBJECT_INFO_DIALOG_H
#define FILEOBJECT_INFO_DIALOG_H

#include <QDialog>
#include <QModelIndex>

class DfxmlListModel;
class DfxmlSource;

namespace Ui {
	class FileObjectInfoDialog;
}

class FileObjectInfoDialog : public QDialog
{
	Q_OBJECT

	public:
		explicit FileObjectInfoDialog(DfxmlListModel *model, const QModelIndex &index, QWidget *parent = 0);
		~FileObjectInfoDialog();

	private slots:
		void on_buttonBox_accepted();
		void on_buttonBox_rejected();

	private:
		Ui::FileObjectInfoDialog *ui;

		DfxmlSource    *m_source;
};

#endif

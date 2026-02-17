#include "include/ui/dialogs/InsertImageDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QDebug>

namespace QtWordEditor {

class InsertImageDialog::Private
{
public:
    QLineEdit *pathEdit = nullptr;
    QPushButton *browseButton = nullptr;
};

InsertImageDialog::InsertImageDialog(QWidget *parent)
    : QDialog(parent)
    , d(new Private)
{
    setWindowTitle(tr("Insert Image"));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *label = new QLabel(tr("Select an image file:"), this);
    mainLayout->addWidget(label);

    QHBoxLayout *pathLayout = new QHBoxLayout();
    d->pathEdit = new QLineEdit(this);
    d->pathEdit->setReadOnly(true);
    d->browseButton = new QPushButton(tr("Browse..."), this);
    pathLayout->addWidget(d->pathEdit);
    pathLayout->addWidget(d->browseButton);
    mainLayout->addLayout(pathLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    QPushButton *okButton = new QPushButton(tr("OK"), this);
    QPushButton *cancelButton = new QPushButton(tr("Cancel"), this);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(d->browseButton, &QPushButton::clicked, [this]() {
        QString fileName = QFileDialog::getOpenFileName(this,
            tr("Open Image"),
            QString(),
            tr("Image Files (*.png *.jpg *.jpeg *.bmp *.gif)"));
        if (!fileName.isEmpty()) {
            d->pathEdit->setText(fileName);
        }
    });
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

InsertImageDialog::~InsertImageDialog()
{
}

QString InsertImageDialog::getImagePath(QWidget *parent)
{
    InsertImageDialog dialog(parent);
    if (dialog.exec() == QDialog::Accepted) {
        return dialog.selectedPath();
    }
    return QString();
}

QString InsertImageDialog::selectedPath() const
{
    return d->pathEdit->text();
}

} // namespace QtWordEditor
#include "WelcomePage.h"

#include <QFont>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QVBoxLayout>

WelcomePage::WelcomePage(QWidget* parent)
    : QWidget(parent)
{
    auto* title = new QLabel("Welcome!", this);
    title->setAlignment(Qt::AlignCenter);
    QFont titleFont = title->font();
    titleFont.setPointSize(40);
    titleFont.setBold(true);
    title->setFont(titleFont);

    auto* effect = new QGraphicsOpacityEffect(title);
    title->setGraphicsEffect(effect);
    auto* fade = new QPropertyAnimation(effect, "opacity", this);
    fade->setDuration(900);
    fade->setStartValue(0.0);
    fade->setEndValue(1.0);
    fade->start();

    auto* subtitle = new QLabel("Plan the work · Match the team · Measure actual progress", this);
    subtitle->setAlignment(Qt::AlignCenter);

    auto* startButton = new QPushButton("+ Start New Project", this);
    auto* continueButton = new QPushButton("Continue Your Projects →", this);
    connect(startButton, &QPushButton::clicked, this, &WelcomePage::startNewProject);
    connect(continueButton, &QPushButton::clicked, this, &WelcomePage::continueProjects);

    auto* buttons = new QHBoxLayout();
    buttons->addStretch();
    buttons->addWidget(startButton);
    buttons->addWidget(continueButton);
    buttons->addStretch();

    auto* layout = new QVBoxLayout(this);
    layout->addStretch();
    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addSpacing(24);
    layout->addLayout(buttons);
    layout->addStretch();
}
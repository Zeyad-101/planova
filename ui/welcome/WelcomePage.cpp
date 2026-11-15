#include "WelcomePage.h"

#include <QFrame>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QVBoxLayout>

WelcomePage::WelcomePage(QWidget* parent)
    : QWidget(parent)
{
    auto* card = new QFrame(this);
    card->setObjectName("welcomeCard");
    card->setMinimumSize(520, 260);

    auto* title = new QLabel("Welcome!", card);
    title->setObjectName("welcomeTitle");
    title->setAlignment(Qt::AlignCenter);

    // Fade-in animation on the title
    auto* effect = new QGraphicsOpacityEffect(title);
    title->setGraphicsEffect(effect);
    auto* fade = new QPropertyAnimation(effect, "opacity", this);
    fade->setDuration(900);
    fade->setStartValue(0.0);
    fade->setEndValue(1.0);
    fade->start();

    auto* subtitle = new QLabel("Plan the work · Match the team · Measure actual progress", card);
    subtitle->setObjectName("welcomeSubtitle");
    subtitle->setAlignment(Qt::AlignCenter);

    auto* startButton = new QPushButton("+ Start New Project", card);
    startButton->setObjectName("primaryButton");
    auto* continueButton = new QPushButton("Continue Your Projects  →", card);
    continueButton->setObjectName("secondaryButton");
    connect(startButton, &QPushButton::clicked, this, &WelcomePage::startNewProject);
    connect(continueButton, &QPushButton::clicked, this, &WelcomePage::continueProjects);

    auto* buttons = new QHBoxLayout();
    buttons->addStretch();
    buttons->addWidget(startButton);
    buttons->addWidget(continueButton);
    buttons->addStretch();

    auto* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(40, 40, 40, 40);
    cardLayout->addWidget(title);
    cardLayout->addWidget(subtitle);
    cardLayout->addSpacing(24);
    cardLayout->addLayout(buttons);

    auto* row = new QHBoxLayout();
    row->addStretch();
    row->addWidget(card);
    row->addStretch();

    auto* layout = new QVBoxLayout(this);
    layout->addStretch();
    layout->addLayout(row);
    layout->addStretch();
}
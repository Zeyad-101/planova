#pragma once
#include <string>

using Id = std::string;

enum class TaskStatus { ToDo, InProgress, Review, Done };
enum class Priority { Low, Medium, High, Critical };
enum class SkillCategory { Backend, Frontend, Storage, UiUx, Integration, Testing, Documentation };
enum class PinCategory { HandoffNote, FeedbackBlocker, ReadyForTesting, ArchitectureDecision };
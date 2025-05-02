CMAKE_MINIMUM_REQUIRED(VERSION 3.30)
PROJECT(
    SavedGameFormat,
    VERSION 0.0.1
    DESCRIPTION "Saved Game formats in C++"
    HOMEPAGE_URL "https://github.com/BenjamenMeyer/saved-game-format"
    LANGUAGES CXX
)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

SUBDIRS(src)

/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef DRACI_DRACI_H
#define DRACI_DRACI_H

#include "engines/engine.h"
#include "common/random.h"
#include "draci/console.h"

struct ADGameDescription;

class MidiDriver;
class OSystem;

/**
 * This is the namespace of the Draci engine.
 *
 * Status of this engine: Complete
 *
 * Games using this engine:
 * - Dragon History
 */
namespace Draci {

enum DRACIAction {
	kActionNone,
	kActionEscape,
	kActionMap,
	kActionShowWalkMap,
	kActionToggleWalkSpeed,
	kActionInventory,
	kActionOpenMainMenu,
	kActionTogglePointerItem,
	kActionInvRotatePrevious,
	kActionInvRotateNext
};

class Screen;
class Mouse;
class Game;
class Script;
class AnimationManager;
class Sound;
class MusicPlayer;
class Font;
class BArchive;
class SoundArchive;

class DraciEngine : public Engine {
public:
	DraciEngine(OSystem *syst, const ADGameDescription *gameDesc);
	~DraciEngine() override;

	int init();
	Common::Error run() override;

	bool hasFeature(Engine::EngineFeature f) const override;
	void pauseEngineIntern(bool pause) override;
	void syncSoundSettings() override;

	void handleEvents();

	static Common::String getSavegameFile(int saveGameIdx);
	Common::String getSaveStateName(int slot) const override { return getSavegameFile(slot); }
	Common::Error loadGameState(int slot) override;
	bool canLoadGameStateCurrently(Common::U32String *msg = nullptr) override;
	Common::Error saveGameState(int slot, const Common::String &desc, bool isAutosave = false) override;
	bool canSaveGameStateCurrently(Common::U32String *msg = nullptr) override;

	Screen *_screen;
	Mouse *_mouse;
	Game *_game;
	Script *_script;
	AnimationManager *_anims;
	Sound *_sound;
	MusicPlayer *_music;

	Font *_smallFont;
	Font *_bigFont;

	BArchive *_iconsArchive;
	BArchive *_objectsArchive;
	BArchive *_spritesArchive;
	BArchive *_paletteArchive;
	BArchive *_roomsArchive;
	BArchive *_overlaysArchive;
	BArchive *_animationsArchive;
	BArchive *_walkingMapsArchive;
	BArchive *_itemsArchive;
	BArchive *_itemImagesArchive;
	BArchive *_initArchive;
	BArchive *_stringsArchive;

	SoundArchive *_soundsArchive;
	SoundArchive *_dubbingArchive;

	bool _showWalkingMap;

	Common::RandomSource _rnd;

	int32 _pauseStartTime;
};

enum {
	kDraciGeneralDebugLevel = 1,
	kDraciBytecodeDebugLevel,
	kDraciArchiverDebugLevel,
	kDraciLogicDebugLevel,
	kDraciAnimationDebugLevel,
	kDraciSoundDebugLevel,
	kDraciWalkingDebugLevel,
};

} // End of namespace Draci

#endif // DRACI_DRACI_H

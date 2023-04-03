/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2023 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "sonic3air/pch.h"
#include "sonic3air/data/SharedDatabase.h"

#include "oxygen/application/Configuration.h"
#include "oxygen/resources/SpriteCache.h"


bool SharedDatabase::mIsInitialized = false;
std::vector<SharedDatabase::Zone> SharedDatabase::mAllZones;
std::vector<SharedDatabase::Zone> SharedDatabase::mAvailableZones;
std::unordered_map<uint32, SharedDatabase::Setting> SharedDatabase::mSettings;
std::vector<SharedDatabase::Achievement> SharedDatabase::mAchievements;
std::map<uint32, SharedDatabase::Achievement*> SharedDatabase::mAchievementMap;
std::vector<SharedDatabase::Secret> SharedDatabase::mSecrets;


void SharedDatabase::initialize()
{
	if (mIsInitialized)
		return;

	// Setup list of zones
	{
		mAllZones.emplace_back("aiz", "zone01_aiz", "Zona Angel Island",	0x00, 2, 2);
		mAllZones.emplace_back("hcz", "zone02_hcz", "Zona Hydrocity",		0x01, 2, 2);
		mAllZones.emplace_back("mgz", "zone03_mgz", "Zona Marble Garden",	0x02, 2, 2);
		mAllZones.emplace_back("cnz", "zone04_cnz", "Zona Carnival Night",	0x03, 2, 2);
		mAllZones.emplace_back("icz", "zone05_icz", "Zona IceCap",			0x05, 2, 2);
		mAllZones.emplace_back("lbz", "zone06_lbz", "Zona Launch Base",		0x06, 2, 2);
		mAllZones.emplace_back("mhz", "zone07_mhz", "Zona Mushroom Hill",	0x07, 2, 2);
		mAllZones.emplace_back("fbz", "zone08_fbz", "Zona Flying Battery",	0x04, 2, 2);
		mAllZones.emplace_back("soz", "zone09_soz", "Zona Sandopolis",		0x08, 2, 2);
		mAllZones.emplace_back("lrz", "zone10_lrz", "Zona Lava Reef",		0x09, 2, 2);
		mAllZones.emplace_back("hpz", "zone11_hpz", "Zona Hidden Palace",	0x16, 1, 0);	// Not for Time Attack
		mAllZones.emplace_back("ssz", "zone12_ssz", "Zona Sky Sanctuary",	0x0a, 1, 1);	// Only Act 1
		mAllZones.emplace_back("dez", "zone13_dez", "Zona Death Egg",		0x0b, 2, 2);
		mAllZones.emplace_back("ddz", "zone14_ddz", "Zona Doomsday",		0x0c, 1, 0);
	}

	// Setup gameplay settings
	setupSettings();

	// Setup achievements
	{
		mAchievements.reserve(10);

		auto addAchievement = [&](Achievement::Type type, const char* name, const char* description, const char* hint, const char* image)
		{
			Achievement& achievement = vectorAdd(mAchievements);
			achievement.mType = type;
			achievement.mName = name;
			achievement.mDescription = description;
			achievement.mHint = hint;
			achievement.mImage = image;
		};

		addAchievement(Achievement::ACHIEVEMENT_300_RINGS,				"Atraido por coisas brilhantes", "Coletar 300 aneis sem perde-los.", "", "rings");
		addAchievement(Achievement::ACHIEVEMENT_DOUBLE_INVINCIBILITY,	"Dose dupla de estrelas", "Abra outro monitor de invencibilidade enquanto ainda estiver invencivel do ultimo.", "", "invincibility");
		addAchievement(Achievement::ACHIEVEMENT_CONTINUES,				"Seguro de vida antiquado", "Ter 5 continues em um unico jogo.", "", "continues");
		addAchievement(Achievement::ACHIEVEMENT_GOING_HYPER,			"Ficando Hiper", "Coletar as 14 esmeraldas e se transformar em uma forma Hiper.", "", "hyperform");
		addAchievement(Achievement::ACHIEVEMENT_SCORE,					"Pontuacao milionaria", "Obter uma pontuacao de 1.000.000 de pontos.", "", "score");
		addAchievement(Achievement::ACHIEVEMENT_ELECTROCUTE,			"Pesca eletrica", "Derrotar um inimigo aquatico por eletrocussao.", "", "electrocution");
		addAchievement(Achievement::ACHIEVEMENT_LONGPLAY,				"Jogatina", "Concluir o jogo com qualquer personagem.", "", "gamebeaten");
		addAchievement(Achievement::ACHIEVEMENT_BS_PERFECT,				"Limpeza do local", "Completar um estagio Esfera Azuis com perfeicao.", "", "perfect");
		addAchievement(Achievement::ACHIEVEMENT_GS_EXIT_TOP,			"Tem uma saida la em cima?", "Chegar ao topo do fase bonus das Esferas Brilhantes.", "", "glowingspheres");
		addAchievement(Achievement::ACHIEVEMENT_SM_JACKPOT,				"Premio acumulado", "Ganhar o premio acumulado no estagio bonus Caca-niqueis.", "", "jackpot");
		addAchievement(Achievement::ACHIEVEMENT_AIZ_TIMEATTACK,			"Cortando caminho pela selva", "Concluir o Ato 1 da zona Angel Island no modo Duelo Crono em menos de 45 segundos.", "", "timeattack_aiz1");
		addAchievement(Achievement::ACHIEVEMENT_MGZ_GIANTRINGS,			"Atraido por coisas gigantes e brilhantes", "Entre ou colete 6 aneis gigantes no Ato 1 da zona Marble Garden em uma unica jogada sem morrer no caminho.", "", "giantrings_mgz1");
		addAchievement(Achievement::ACHIEVEMENT_ICZ_SNOWBOARDING,		"Snowboarder ganancioso", "Coletar todos os 50 aneis na secao de snowboarding do Ato 1 da zona IceCap.", "", "snowboarding");
		addAchievement(Achievement::ACHIEVEMENT_ICZ_KNUX_SUNRISE,		"Ver o nascer do sol uma vez", "Derrotar o chefe superior no Ato 1 da zona IceCap com Knuckles (talvez precise de um amigo para isso).", "", "icecap1boss");
		addAchievement(Achievement::ACHIEVEMENT_LBZ_STAY_DRY,			"Pelagem fofa nao pode ficar molhada", "Concluir o Ato 2 da zona Launch Base sem tocar em qualquer agua (requer o layout do nivel A.I.R).", "", "staydry");
		addAchievement(Achievement::ACHIEVEMENT_MHZ_OPEN_MONITORS,		"Quebra de monitores", "Abra 18 monitores no Ato 1 da zona Mushroom Hill 1 com Knuckles.", "", "monitors");
		addAchievement(Achievement::ACHIEVEMENT_FBZ_FREE_ANIMALS,		"Esquilos em um aviao", "Liberte 35 animais no Ato 1 da zona Flying Battery antes do chefe.", "", "animals");
		addAchievement(Achievement::ACHIEVEMENT_SSZ_DECOYS,				"Nao toque", "Lute contra o segundo chefe no Sky Sanctuary do Sonic, mas estoure no maximo um dos Mechas inflaveis.", "", "decoys");

		for (Achievement& achievement : mAchievements)
		{
			mAchievementMap[achievement.mType] = &achievement;
		}
	}

	// Setup secrets
	{
		mSecrets.reserve(4);

		auto addSecret = [&](Secret::Type type, bool hiddenUntilUnlocked, bool shownInMenu, bool serialized, uint32 requiredAchievements, const char* name, const char* description, const char* image)
		{
			Secret& secret = vectorAdd(mSecrets);
			secret.mType = type;
			secret.mName = name;
			secret.mDescription = description;
			secret.mImage = image;
			secret.mRequiredAchievements = requiredAchievements;
			secret.mUnlockedByAchievements = (requiredAchievements > 0);
			secret.mHiddenUntilUnlocked = hiddenUntilUnlocked;
			secret.mShownInMenu = shownInMenu;
			secret.mSerialized = serialized;
		};

		addSecret(Secret::SECRET_COMPETITION_MODE,	false, true,  false,  0, "Modo competicao", "Como conhecido no Sonic 3 original (& Knuckles).", "competitionmode");
		addSecret(Secret::SECRET_DROPDASH,			false, true,  true,   3, "Sonic Drop Dash", "No menu Opcoes (em Controles), voce agora pode ativar o movimento Drop Dash do Sonic para o modo Jogo Normal e Escolha de Ato", "dropdash");
		addSecret(Secret::SECRET_KNUX_AND_TAILS,	false, true,  true,   5, "Modo Knuckles & Tails", "Jogue com a combinacao de personagens Knuckles e Tails no modo Jogo Normal e Escolha de Ato.", "knuckles_tails");
		addSecret(Secret::SECRET_SUPER_PEELOUT,		false, true,  true,   7, "Sonic Super Arrancada", "O movimento Super Arrancada esta disponivel no menu Opcoes. Isso tambem desbloqueia o modo de jogo \"Controle Maximo\" na Escolha de Ato.", "superpeelout");
		addSecret(Secret::SECRET_DEBUGMODE,			false, true,  true,  10, "Mode depuracao", "O Debug Mode depuracao pode ser ativado no menu Opcoes (em Ajustes), e esta disponivel no modo Jogo Normal e Escolha de Ato.", "debugmode");
		addSecret(Secret::SECRET_BLUE_SPHERE,		false, true,  true,  12, "Esfera azul", "Adiciona o jogo Esfera Azul aos Extras que e conhecido por estar presente quando se conecta o jogo Sonic 1 ao Sonic & Knuckles.", "bluesphere");
		addSecret(Secret::SECRET_LEVELSELECT,		true,  true,  true,   0, "Selecao de Nivel", "Adiciona o menu de Selecao de Nivel original do Sonic 3 & Knuckles aos Extras.", "levelselect");
		addSecret(Secret::SECRET_TITLE_SK,			true,  true,  true,   0, "Titulo Sonic & Knuckles", "Agora voce pode selecionar a tela de titulo do Sonic & Knuckles no menu de Opcoes.", "title_sk");
		addSecret(Secret::SECRET_GAME_SPEED,		true,  true,  true,   0, "Velocidade do jogo", "Pronto para um novo desafio? Ajuste a velocidade do jogo para mais rapido ou mais lento no menu de Opcoes.", "gamespeed");
		addSecret(Secret::SECRET_DOOMSDAY_ZONE,		true,  false, true,   0, "Zana Doomsday", "", "");
	}

	mIsInitialized = true;
}

const SharedDatabase::Zone* SharedDatabase::getZoneByInternalIndex(uint8 index)
{
	for (const SharedDatabase::Zone& zone : SharedDatabase::getAllZones())
	{
		if (zone.mInternalIndex == index)
		{
			return &zone;
		}
	}
	return nullptr;
}

uint64 SharedDatabase::setupCharacterSprite(EmulatorInterface& emulatorInterface, uint8 character, uint16 animationSprite, bool superActive)
{
	if (animationSprite >= 0x100)
	{
		// Special handling required
		if (animationSprite >= 0x102)
		{
			return rmx::getMurmur2_64(String(0, "sonic_peelout_%d", animationSprite - 0x102));
		}
		else
		{
			return rmx::getMurmur2_64(String(0, "sonic_dropdash_%d", animationSprite - 0x100));
		}
	}
	else
	{
		uint32 sourceBase;
		uint32 tableAddress;
		uint32 mappingOffset;
		switch (character)
		{
			default:
			case 0:		// Sonic
				sourceBase    = (animationSprite >= 0xda) ? 0x140060 : 0x100000;
				tableAddress  = (superActive) ? 0x148378 : 0x148182;
				mappingOffset = (superActive) ? 0x146816 : 0x146620;
				break;

			case 1:		// Tails
				sourceBase    = (animationSprite >= 0xd1) ? 0x143d00 : 0x3200e0;
				tableAddress  = 0x14a08a;
				mappingOffset = 0x148eb8;
				break;

			case 2:		// Knuckles
				sourceBase    = 0x1200e0;
				tableAddress  = 0x14bd0a;
				mappingOffset = 0x14a8d6;
				break;
		}

		return SpriteCache::instance().setupSpriteFromROM(emulatorInterface, sourceBase, tableAddress, mappingOffset, (uint8)animationSprite, 0x00, SpriteCache::ENCODING_CHARACTER);
	}
}

uint64 SharedDatabase::setupTailsTailsSprite(EmulatorInterface& emulatorInterface, uint8 animationSprite)
{
	return SpriteCache::instance().setupSpriteFromROM(emulatorInterface, 0x336620, 0x344d74, 0x344bb8, animationSprite, 0x00, SpriteCache::ENCODING_CHARACTER);
}

uint8 SharedDatabase::getTailsTailsAnimationSprite(uint8 characterAnimationSprite, uint32 globalTime)
{
	// Translate main character sprite into tails sprite
	//  -> Note that is only an estimation and does not represent the actual calculation by game
	if (characterAnimationSprite >= 0x86 && characterAnimationSprite <= 0x88)
	{
		// Spindash
		return 0x01 + (globalTime / 3) % 4;
	}
	else if (characterAnimationSprite >= 0x96 && characterAnimationSprite <= 0x98)
	{
		// Rolling
		return 0x05 + (globalTime / 4) % 4;
	}
	else if (characterAnimationSprite == 0x99 || (characterAnimationSprite >= 0xad && characterAnimationSprite <= 0xb4))
	{
		// Standing -- including idle anim, looking up/down
		return 0x22 + (globalTime / 8) % 5;
	}
	else if (characterAnimationSprite == 0xa0)
	{
		// Flying
		// TODO: When flying down, this is updated only every two frames, not every frame
		return 0x27 + (globalTime) % 2;
	}
	return 0;
}

const SharedDatabase::Setting* SharedDatabase::getSetting(uint32 settingId)
{
	const auto it = mSettings.find(settingId);
	return (it == mSettings.end()) ? nullptr : &it->second;
}

uint32 SharedDatabase::getSettingValue(uint32 settingId)
{
	const SharedDatabase::Setting* setting = getSetting(settingId);
	if (nullptr != setting)
	{
		return setting->mCurrentValue;
	}

	// Use default value
	return (settingId & 0xff);
}

SharedDatabase::Achievement* SharedDatabase::getAchievement(uint32 achievementId)
{
	return mAchievementMap[achievementId];
}

const std::vector<SharedDatabase::Achievement>& SharedDatabase::getAchievements()
{
	return mAchievements;
}

void SharedDatabase::resetAchievementValues()
{
	for (Achievement& achievement : mAchievements)
	{
		achievement.mValue = 0;
	}
}

SharedDatabase::Secret* SharedDatabase::getSecret(uint32 secretId)
{
	// No additional std::map used to optimize this, as the number of secrets is very low
	for (Secret& secret : mSecrets)
	{
		if (secret.mType == secretId)
			return &secret;
	}
	return nullptr;
}

SharedDatabase::Setting& SharedDatabase::addSetting(SharedDatabase::Setting::Type id, const char* identifier, SharedDatabase::Setting::SerializationType serializationType, bool enforceAllowInTimeAttack)
{
	SharedDatabase::Setting& setting = mSettings[(uint32)id];
	setting.mSettingId = id;
	setting.mIdentifier = std::string(identifier).substr(9);
	setting.mCurrentValue = ((uint32)id & 0xff);
	setting.mDefaultValue = ((uint32)id & 0xff);
	setting.mSerializationType = serializationType;
	setting.mPurelyVisual = ((uint32)id & 0x80000000) != 0;
	setting.mAllowInTimeAttack = enforceAllowInTimeAttack || setting.mPurelyVisual;
	return setting;
};

void SharedDatabase::setupSettings()
{
	#define IDPARAMS(_id_) _id_, #_id_

	// These settings get saved in "settings.json" under their setting ID
	addSetting(IDPARAMS(Setting::SETTING_FIX_GLITCHES), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_NO_CONTROL_LOCK), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_TAILS_ASSIST_MODE), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_CANCEL_FLIGHT), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_SUPER_CANCEL), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_INSTA_SHIELD), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_HYPER_TAILS), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_SHIELD_TYPES), Setting::SerializationType::ALWAYS);

	addSetting(IDPARAMS(Setting::SETTING_AIZ_BLIMPSEQUENCE), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_LBZ_BIGARMS), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_LRZ2_BOSS), Setting::SerializationType::ALWAYS);

	addSetting(IDPARAMS(Setting::SETTING_EXTENDED_HUD), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_SMOOTH_ROTATION), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_SPEEDUP_AFTERIMGS), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_BS_VISUAL_STYLE), Setting::SerializationType::ALWAYS);

	addSetting(IDPARAMS(Setting::SETTING_INFINITE_LIVES), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_INFINITE_TIME), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_RANDOM_MONITORS), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_RANDOM_SPECIALSTAGES), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_BUBBLE_SHIELD_BOUNCE), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_CAMERA_OUTRUN), Setting::SerializationType::ALWAYS, true);			// Allowed in Time Attack, even though it's not purely visual (it has a minimal impact on gameplay simulation)
	addSetting(IDPARAMS(Setting::SETTING_EXTENDED_CAMERA), Setting::SerializationType::ALWAYS, true);		// Same here
	addSetting(IDPARAMS(Setting::SETTING_MAINTAIN_SHIELDS), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_BS_REPEAT_ON_FAIL), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_DISABLE_GHOST_SPAWN), Setting::SerializationType::ALWAYS);

	addSetting(IDPARAMS(Setting::SETTING_SUPERFAST_RUNANIM), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_MONITOR_STYLE), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_HYPER_DASH_CONTROLS), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_SUPER_SONIC_ABILITY), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_MONITOR_BEHAVIOR), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_LIVES_DISPLAY), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_BS_COUNTDOWN_RINGS), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_CONTINUE_MUSIC), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_UNDERWATER_AUDIO), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_ICZ_NIGHTTIME), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_CNZ_PROTOTYPE_MUSIC), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_ICZ_PROTOTYPE_MUSIC), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_LBZ_PROTOTYPE_MUSIC), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_FBZ2_MIDBOSS_TRACK), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_SSZ_BOSS_TRACKS), Setting::SerializationType::ALWAYS);

	addSetting(IDPARAMS(Setting::SETTING_GFX_ANTIFLICKER), Setting::SerializationType::ALWAYS, true);		// Allowed in Time Attack
	addSetting(IDPARAMS(Setting::SETTING_LEVELLAYOUTS), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_REGION_CODE), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_TIME_ATTACK_GHOSTS), Setting::SerializationType::ALWAYS);

	addSetting(IDPARAMS(Setting::SETTING_AUDIO_TITLE_THEME), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_AUDIO_EXTRALIFE_JINGLE), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_AUDIO_INVINCIBILITY_THEME), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_AUDIO_SUPER_THEME), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_AUDIO_MINIBOSS_THEME), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_AUDIO_KNUCKLES_THEME), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_AUDIO_HPZ_MUSIC), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_AUDIO_OUTRO), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_AUDIO_COMPETITION_MENU), Setting::SerializationType::ALWAYS);
	addSetting(IDPARAMS(Setting::SETTING_AUDIO_CONTINUE_SCREEN), Setting::SerializationType::ALWAYS);

	// Hidden settings
	addSetting(IDPARAMS(Setting::SETTING_DROPDASH), Setting::SerializationType::HIDDEN);
	addSetting(IDPARAMS(Setting::SETTING_SUPER_PEELOUT), Setting::SerializationType::HIDDEN);
	addSetting(IDPARAMS(Setting::SETTING_DEBUG_MODE), Setting::SerializationType::HIDDEN);
	addSetting(IDPARAMS(Setting::SETTING_TITLE_SCREEN), Setting::SerializationType::HIDDEN);

	// Not saved at all
	addSetting(IDPARAMS(Setting::SETTING_KNUCKLES_AND_TAILS), Setting::SerializationType::NONE);

	#undef IDPARAMS
}

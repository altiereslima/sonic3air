/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2023 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "sonic3air/pch.h"
#include "sonic3air/menu/options/OptionsMenuEntries.h"
#include "sonic3air/menu/options/OptionsEntry.h"
#include "sonic3air/menu/options/OptionsMenu.h"
#include "sonic3air/menu/SharedResources.h"
#include "sonic3air/audio/AudioOut.h"
#include "sonic3air/client/GameClient.h"
#include "sonic3air/client/UpdateCheck.h"
#include "sonic3air/ConfigurationImpl.h"
#include "sonic3air/Game.h"
#include "sonic3air/version.inc"

#include "oxygen/application/Application.h"
#include "oxygen/download/Downloader.h"


namespace
{
	const String& getVersionString(uint32 buildNumber)
	{
		static uint32 cachedBuildNumber = 0xffffffff;
		static String cachedBuildString;
		if (cachedBuildNumber != buildNumber || cachedBuildNumber == 0xffffffff)
		{
			cachedBuildNumber = buildNumber;
			cachedBuildString.format("v%02x.%02x.%02x.%x", (buildNumber >> 24) & 0xff, (buildNumber >> 16) & 0xff, (buildNumber >> 8) & 0xff, buildNumber & 0xff);
		}
		return cachedBuildString;
	}
}


TitleMenuEntry::TitleMenuEntry()
{
	mMenuEntryType = MENU_ENTRY_TYPE;
	setInteractable(false);
}

TitleMenuEntry& TitleMenuEntry::initEntry(const std::string& text)
{
	mText = text;
	return *this;
}

void TitleMenuEntry::renderEntry(RenderContext& renderContext_)
{
	OptionsMenuRenderContext& renderContext = renderContext_.as<OptionsMenuRenderContext>();
	Drawer& drawer = *renderContext.mDrawer;
	const int baseX = renderContext.mCurrentPosition.x;
	int& py = renderContext.mCurrentPosition.y;

	py += 15;
	drawer.printText(global::mSonicFontB, Recti(baseX, py, 0, 10), ("* " + mText + " *"), 5, Color(0.6f, 0.8f, 1.0f, renderContext.mTabAlpha));
	py += 2;
}


SectionMenuEntry::SectionMenuEntry()
{
	mMenuEntryType = MENU_ENTRY_TYPE;
	setInteractable(false);
}

SectionMenuEntry& SectionMenuEntry::initEntry(const std::string& text)
{
	mText = text;
	return *this;
}

void SectionMenuEntry::renderEntry(RenderContext& renderContext_)
{
	OptionsMenuRenderContext& renderContext = renderContext_.as<OptionsMenuRenderContext>();
	Drawer& drawer = *renderContext.mDrawer;
	const int baseX = renderContext.mCurrentPosition.x;
	int& py = renderContext.mCurrentPosition.y;
	const float alpha = renderContext.mTabAlpha;

	py += 14;
	const int textWidth = global::mOxyfontRegular.getWidth(mText);
	drawer.printText(global::mOxyfontRegular, Recti(baseX - 140, py, 0, 10), mText, 4, Color(0.7f, 1.0f, 0.9f, alpha));
	drawer.drawRect(Recti(baseX - 185, py + 4, 40, 1), Color(0.7f, 1.0f, 0.9f, alpha));
	drawer.drawRect(Recti(baseX - 184, py + 5, 40, 1), Color(0.0f, 0.0f, 0.0f, alpha * 0.75f));
	drawer.drawRect(Recti(baseX - 135 + textWidth, py + 4, 320 - textWidth, 1), Color(0.7f, 1.0f, 0.9f, alpha));
	drawer.drawRect(Recti(baseX - 134 + textWidth, py + 5, 320 - textWidth, 1), Color(0.0f, 0.0f, 0.0f, alpha * 0.75f));
	py += 7;
}


LabelMenuEntry::LabelMenuEntry()
{
	setInteractable(false);
}

LabelMenuEntry& LabelMenuEntry::initEntry(const std::string& text, const Color& color)
{
	mText = text;
	mColor = color;
	return *this;
}

void LabelMenuEntry::renderEntry(RenderContext& renderContext_)
{
	OptionsMenuRenderContext& renderContext = renderContext_.as<OptionsMenuRenderContext>();
	Drawer& drawer = *renderContext.mDrawer;
	const int baseX = renderContext.mCurrentPosition.x;
	int& py = renderContext.mCurrentPosition.y;

	py -= 1;
	const Vec2i boxSize = global::mOxyfontTiny.getTextBoxSize(mText);
	drawer.printText(global::mOxyfontTiny, Recti(baseX, py, 0, 10), mText, 5, Color(mColor.r, mColor.g, mColor.b, mColor.a * renderContext.mTabAlpha));
	py += boxSize.y - 4;
}


OptionsMenuEntry& OptionsMenuEntry::setUseSmallFont(bool useSmallFont)
{
	mUseSmallFont = useSmallFont;
	return *this;
}

void OptionsMenuEntry::renderEntry(RenderContext& renderContext_)
{
	renderInternal(renderContext_, Color::WHITE, Color::YELLOW);
}

void OptionsMenuEntry::renderInternal(RenderContext& renderContext_, const Color& normalColor, const Color& selectedColor)
{
	OptionsMenuRenderContext& renderContext = renderContext_.as<OptionsMenuRenderContext>();
	Drawer& drawer = *renderContext.mDrawer;
	const int baseX = renderContext.mCurrentPosition.x;
	int& py = renderContext.mCurrentPosition.y;
	Font& font = (mUseSmallFont || renderContext.mIsModsTab) ? global::mOxyfontSmall : global::mOxyfontRegular;

	const bool isSelected = renderContext.mIsSelected;
	const bool isDisabled = !isInteractable();

	Color color = isSelected ? selectedColor : isDisabled ? Color(0.4f, 0.4f, 0.4f) : normalColor;
	color.a *= renderContext.mTabAlpha;

	if (mOptions.empty())
	{
		// Used for selectable entries, like "Back"
		if (mData == option::_BACK)
		{
			py += 16;
		}

		if (mData == option::CONTROLLER_SETUP)
		{
			drawer.printText(font, Recti(baseX, py, 0, 10), Application::instance().hasKeyboard() ? "Configurar teclado e controle..." : "Configurar controle...", 5, color);
		}
		else
		{
			drawer.printText(font, Recti(baseX, py, 0, 10), mText, 5, color);
		}

		if (isSelected)
		{
			// Draw arrows
			const int halfTextWidth = font.getWidth(mText) / 2;
			const int offset = (int)std::fmod(FTX::getTime() * 6.0f, 6.0f);
			const int arrowDistance = 16 + ((offset > 3) ? (6 - offset) : offset);
			drawer.printText(font, Recti(baseX - halfTextWidth - arrowDistance, py, 0, 10), ">>", 5, color);
			drawer.printText(font, Recti(baseX + halfTextWidth + arrowDistance, py, 0, 10), "<<", 5, color);
		}

		if (mData == option::CONTROLLER_SETUP)
			py += 4;
	}
	else
	{
		// It's an actual options entry, with multiple options to choose from
		const bool canGoLeft  = !isDisabled && (mSelectedIndex > 0);
		const bool canGoRight = !isDisabled && (mSelectedIndex < mOptions.size() - 1);

		const int center = mText.empty() ? baseX : (baseX + 88);
		int arrowDistance = 75;
		if (isSelected)
		{
			const int offset = (int)std::fmod(FTX::getTime() * 6.0f, 6.0f);
			arrowDistance += ((offset > 3) ? (6 - offset) : offset);
		}

		// Description
		if (!mText.empty())
		{
			drawer.printText(font, Recti(baseX - 40, py, 0, 10), mText, 6, color);
		}

		// Value text
		const AudioCollection::AudioDefinition* audioDefinition = nullptr;
		{
			static const std::string TEXT_NOT_AVAILABLE = "nao disponivel";
			const std::string* text = (isDisabled && mData != option::RENDERER) ? &TEXT_NOT_AVAILABLE : &mOptions[mSelectedIndex].mText;
			if (mData == option::SOUND_TEST)
			{
				audioDefinition = renderContext.mOptionsMenu->getSoundTestAudioDefinition(selected().mValue);
				if (nullptr != audioDefinition && AudioOut::instance().getAudioKeyType(audioDefinition->mKeyId) == AudioOutBase::AudioKeyType::MODDED)
				{
					static std::string combinedText;
					combinedText = *text + " (modded)";
					text = &combinedText;
				}
			}
			drawer.printText(font, Recti(center - 80, py, 160, 10), *text, 5, color);
		}

		if (canGoLeft)
			drawer.printText(font, Recti(center - arrowDistance, py, 0, 10), "<", 5, color);
		if (canGoRight)
			drawer.printText(font, Recti(center + arrowDistance, py, 0, 10), ">", 5, color);

		// Additional text for sound test
		if (mData == option::SOUND_TEST && nullptr != audioDefinition)
		{
			py += 13;
			drawer.printText(global::mOxyfontTiny, Recti(center - 80, py, 160, 10), audioDefinition->mDisplayName, 5, color);
		}
	}
}


AdvancedOptionMenuEntry::AdvancedOptionMenuEntry()
{
	setUseSmallFont(true);
}

void AdvancedOptionMenuEntry::renderEntry(RenderContext& renderContext_)
{
	OptionsMenuRenderContext& renderContext = renderContext_.as<OptionsMenuRenderContext>();
	const Color normalColor = (mOptions[mSelectedIndex].mValue == mDefaultValue) ? Color::WHITE : Color(1.0f, 0.5f, 0.5f);
	const Color selectedColor = (mOptions[mSelectedIndex].mValue == mDefaultValue) ? Color::YELLOW : Color(1.0f, 0.75f, 0.0f);
	renderInternal(renderContext_, normalColor, selectedColor);
	renderContext.mCurrentPosition.y -= 1;
}


void UpdateCheckMenuEntry::renderEntry(RenderContext& renderContext_)
{
	OptionsMenuRenderContext& renderContext = renderContext_.as<OptionsMenuRenderContext>();
	Drawer& drawer = *renderContext.mDrawer;
	const int baseX = renderContext.mCurrentPosition.x;
	int& py = renderContext.mCurrentPosition.y;
	const float alpha = renderContext.mTabAlpha;

	drawer.printText(global::mOxyfontSmall, Recti(baseX - 100, py, 0, 10), "Sua versao do jogo:", 4, Color(1.0f, 1.0f, 1.0f, alpha));
	drawer.printText(global::mOxyfontSmall, Recti(baseX + 100, py, 0, 10), "v" BUILD_STRING, 6, Color(0.8f, 1.0f, 0.8f, alpha));
	py += 12;

	UpdateCheck& updateCheck = GameClient::instance().getUpdateCheck();
	switch (updateCheck.getState())
	{
		case UpdateCheck::State::FAILED:
		{
			drawer.printText(global::mOxyfontSmall, Recti(baseX, py, 0, 10), "Nao e possivel conectar-se ao servidor", 5, Color(1.0f, 0.0f, 0.0f, alpha));
			break;
		}
		case UpdateCheck::State::SEND_QUERY:
		case UpdateCheck::State::WAITING_FOR_RESPONSE:
		{
			drawer.printText(global::mOxyfontSmall, Recti(baseX, py, 0, 10), "Conectando-se ao servidor...", 5, Color(1.0f, 1.0f, 1.0f, alpha));
			break;
		}
		case UpdateCheck::State::HAS_RESPONSE:
		{
			if (updateCheck.hasUpdate())
			{
				drawer.printText(global::mOxyfontSmall, Recti(baseX - 100, py, 0, 10), "Atualizacao disponivel:", 4, Color(1.0f, 1.0f, 1.0f, alpha));
				drawer.printText(global::mOxyfontSmall, Recti(baseX + 100, py, 0, 10), getVersionString(updateCheck.getResponse()->mAvailableAppVersion), 6, Color(1.0f, 1.0f, 0.6f, alpha));
			}
			else
			{
				drawer.printText(global::mOxyfontSmall, Recti(baseX, py, 0, 10), "Voce esta usando a versao mais recente", 5, Color(0.8f, 1.0f, 0.8f, alpha));
			}
			break;
		}
		default:
		{
			drawer.printText(global::mOxyfontSmall, Recti(baseX, py, 0, 10), "Pronto para verificar as atualizacoes", 5, Color(0.8f, 0.8f, 0.8f, alpha));
			break;
		}
	}
	py += 20;

	const bool useTextUpdateLink = updateCheck.hasUpdate();
	if (mTextUpdateLink != useTextUpdateLink)
	{
		mTextUpdateLink = useTextUpdateLink;
		mText = useTextUpdateLink ? "Abrir pagina de download" : "Verificar por atualizacoes";
	}

	OptionsMenuEntry::renderEntry(renderContext);
}

void SoundtrackMenuEntry::renderEntry(RenderContext& renderContext_)
{
	renderInternal(renderContext_, Color::WHITE, Color::YELLOW);

	#if defined(PLATFORM_WEB)
		// Extra text for Web version if remastered soundtrack was not downloaded
		//  -> This could be obsolete if Web can implement the internal downloader functionality
		if (!Downloader::isDownloaderSupported() && !AudioOut::instance().hasLoadedRemasteredSoundtrack() && selected().mIndex == 1)
		{
			OptionsMenuRenderContext& renderContext = renderContext_.as<OptionsMenuRenderContext>();
			Drawer& drawer = *renderContext.mDrawer;
			const int baseX = renderContext.mCurrentPosition.x;
			int& py = renderContext.mCurrentPosition.y;
			const int center = mText.empty() ? baseX : (baseX + 88);

			py += 13;
			drawer.printText(global::mOxyfontTiny, Recti(center - 80, py, 160, 10), "Deve ser baixado separadamente", 5, Color(1.0f, 0.9f, 0.8f, renderContext.mTabAlpha));
			py += 10;
			drawer.printText(global::mOxyfontTiny, Recti(center - 80, py, 160, 10), "(Saia e selecione \"Downloads Extra\")", 5, Color(1.0f, 0.9f, 0.8f, renderContext.mTabAlpha));
			++py;
		}
	#endif
}

void SoundtrackDownloadMenuEntry::renderEntry(RenderContext& renderContext_)
{
	RemasteredMusicDownload& download = Game::instance().getRemasteredMusicDownload();
	const RemasteredMusicDownload::State state = download.getState();
	if (state != RemasteredMusicDownload::State::LOADED)
	{
		OptionsMenuRenderContext& renderContext = renderContext_.as<OptionsMenuRenderContext>();
		Drawer& drawer = *renderContext.mDrawer;
		const int center = renderContext.mCurrentPosition.x;
		int& py = renderContext.mCurrentPosition.y;
		Color color = renderContext.mIsSelected ? Color::YELLOW : Color::WHITE;
		color.a *= renderContext.mTabAlpha;

		std::string text;
		switch (state)
		{
			case RemasteredMusicDownload::State::READY_FOR_DOWNLOAD:
				text = "Baixar a trilha sonora remasterizada agora? (126 MB)";
				mText = "Iniciar download";
				break;

			case RemasteredMusicDownload::State::DOWNLOAD_PENDING:
				text = "Aguardando o download...";
				mText = "Parar download";
				break;

			case RemasteredMusicDownload::State::DOWNLOAD_RUNNING:
				text = "Baixando... " + std::to_string(download.getBytesDownloaded() / (1024*1024)) + " MB";
			#if defined(PLATFORM_ANDROID)
				text += "  (Wi-fi requerido)";
			#endif
				mText = "Parar download";
				break;

			case RemasteredMusicDownload::State::DOWNLOAD_DONE:
				text = "Download completo";
				mText = "Carregar a trilha sonora";
				break;

			case RemasteredMusicDownload::State::DOWNLOAD_FAILED:
				text = "Falha no download";
				mText = "Reiniciar download";
				break;

			default:
				break;
		}

		drawer.printText(global::mOxyfontTiny, Recti(center - 80, py, 160, 10), text, 5, Color(0.8f, 1.0f, 0.9f, color.a));
		py += 12;

		mUseSmallFont = true;
		renderInternal(renderContext_, Color::WHITE, Color::YELLOW);
		py += 3;
	}
}

void SoundtrackDownloadMenuEntry::triggerButton()
{
	RemasteredMusicDownload& download = Game::instance().getRemasteredMusicDownload();
	const RemasteredMusicDownload::State state = download.getState();
	switch (state)
	{
		case RemasteredMusicDownload::State::READY_FOR_DOWNLOAD:
		case RemasteredMusicDownload::State::DOWNLOAD_FAILED:
			download.startDownload();
			break;

		case RemasteredMusicDownload::State::DOWNLOAD_PENDING:
		case RemasteredMusicDownload::State::DOWNLOAD_RUNNING:
			download.removeDownload();
			break;

		case RemasteredMusicDownload::State::DOWNLOAD_DONE:
			download.applyAfterDownload();
			break;

		default:
			break;
	}
}

bool SoundtrackDownloadMenuEntry::shouldBeShown()
{
	return (ConfigurationImpl::instance().mActiveSoundtrack == 1 && Downloader::isDownloaderSupported() && !AudioOut::instance().hasLoadedRemasteredSoundtrack());
}

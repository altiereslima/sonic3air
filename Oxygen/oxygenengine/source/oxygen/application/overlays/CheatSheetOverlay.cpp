/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2023 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "oxygen/pch.h"
#include "oxygen/application/overlays/CheatSheetOverlay.h"
#include "oxygen/application/EngineMain.h"


CheatSheetOverlay::CheatSheetOverlay()
{
}

CheatSheetOverlay::~CheatSheetOverlay()
{
}

void CheatSheetOverlay::initialize()
{
}

void CheatSheetOverlay::deinitialize()
{
}

void CheatSheetOverlay::update(float timeElapsed)
{
	if (mShouldBeShown)
	{
		mVisibility = std::min(mVisibility + timeElapsed / 0.1f, 1.0f);
	}
	else
	{
		mVisibility = std::max(mVisibility - timeElapsed / 0.1f, 0.0f);
	}
}

void CheatSheetOverlay::render()
{
	GuiBase::render();

	if (mVisibility <= 0.0f)
		return;
	const float alpha = mVisibility;
	const Color alphaWhite(1.0f, 1.0f, 1.0f, alpha);

	Drawer& drawer = EngineMain::instance().getDrawer();
	Font& font = EngineMain::getDelegate().getDebugFont(10);

	static const std::vector<const char*> texts =
	{
		// Always available
		"Alt+Enter", "Alternar tela cheia",
		"Alt+F/G",   "Mudar metodo de redimensionamento",
		"Alt+H",     "Mudar metodo de sincronia de quadros",
		"Alt+B",     "Mudar desfoque de fundo",
		"Alt+P",     "Mudar exibicao de desempenho",
		"F2",        "Salvar gravacao do jogo para depuracao",
		"F3",        "Pesquisar controles conectados",
		"F4",        "Trocar controles do jogador 1/2",

		// Dev mode only
		"F5",        "Salvar estado",
		"F7",        "Recarregar ultimo estado",
		"F8",        "Carregar estado",
		"F10",       "Recarregar recursos",
		"F11",       "Recarregar scripts",
		"0..9",      "Teclas de depuracao (podem ser consultadas em scripts)",
		",",         "Mostrar conteudo do plano B",
		".",         "Mostrar conteudo do plano A",
		"-",         "Mostrar conteudo da VRAM",
		"Tab",       "Despejar plano, VRAM ou paleta mostrados",
		"Alt+1..8",  "Alternar renderizacao de camada",
		"Alt+M",     "Alternar exibicao de paleta",
		"Alt+R",     "Mudar metodo de renderizacao",
		"Alt+T",     "Alternar renderizacao de nivel abstraido",
		"Alt+V",     "Alternar visualizacao de depuracao",
		"Alt+C",     "Mudar entre visualizacoes de depuracao",
	};
	const size_t NUM_TEXTS_NONDEV = 8;
	const size_t NUM_TEXTS = EngineMain::getDelegate().useDeveloperFeatures() ? (texts.size() / 2) : NUM_TEXTS_NONDEV;

	mRect.setSize(330, (float)(58 + NUM_TEXTS * 18));
	mRect.setPos(((float)FTX::screenWidth() - mRect.width) * 0.95f, ((float)FTX::screenHeight() - mRect.height) * (1.0f - alpha * 0.1f));
	drawer.drawRect(mRect, Color(0.1f, 0.1f, 0.1f, alpha * 0.6f));

	Recti rct(roundToInt(mRect.x) + 20, roundToInt(mRect.y) + 16, 40, 20);
	drawer.printText(font, rct, "Visao geral de teclas de atalho - exibir/ocultar com F1", 1, Color(0.5f, 1.0f, 1.0f, alpha));
	rct.y += 26;

	for (size_t i = 0; i < NUM_TEXTS; ++i)
	{
		if (i == NUM_TEXTS_NONDEV)
			rct.y += 8;
		drawer.printText(font, rct, texts[i*2], 1, alphaWhite);
		drawer.printText(font, rct + Vec2i(65, 0), texts[i*2+1], 1, alphaWhite);
		rct.y += 18;
	}
}

"""Stage a genuine title CO-OP entry; host/join remain explicitly unavailable."""
import argparse,hashlib,json,pathlib,tempfile
from prepare_probe import BASE_FILES,GAME_FILES,once
from prepare_probe4 import prepare as prepare_probe4
HERE=pathlib.Path(__file__).resolve().parent
PACKAGE='org.bocoop.black.roomui'
FILES=BASE_FILES+GAME_FILES+('src/game/exit_menu.cpp',)

def prepare(root:pathlib.Path):
 root=root.resolve()
 originals={name:(root/name).read_bytes() for name in FILES}
 with tempfile.TemporaryDirectory(prefix='bo-room-ui-') as d:
  stage=pathlib.Path(d)
  for name,data in originals.items():
   p=stage/name;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
  changes=prepare_probe4(stage)
  pending={name:(stage/name).read_text(encoding='utf-8') for name in changes}
 for name in pending:
  pending[name]=pending[name].replace('org.bocoop.black.probe4',PACKAGE).replace('BLACK Probe 4 - READ ONLY','BLACK Co-op UI TEST').replace('BLACK Probe 4','BLACK Co-op UI TEST')
 for name in ('room_route.hpp','room_ui.h'):
  pending['src/coop/'+name]=(HERE/'room_ui'/name).read_text()
 title='src/game/exit_menu.cpp';text=originals[title].decode().replace('\r\n','\n')
 text=once(text,'#include "game/flash.h"','#include "game/flash.h"\n#include "coop/room_ui.h"',title)
 text=once(text,'constexpr char kButtonDepth[] = "100";', '''constexpr char kButtonDepth[] = "100";
constexpr char kCoopButton[] = "CoopButton";
uint32_t g_buttons_root = 0, g_buttons_quit = 0, g_buttons_coop = 0;''',title)
 text=once(text,'uint32_t CopyButton(X86Context& ctx, const TitleMenu& title, uint32_t holder) {',
 'uint32_t CopyButton(X86Context& ctx, const TitleMenu& title, uint32_t holder, const char* name, const char* depth) {',title)
 text=once(text,'flash::Text(kButtonName), flash::Text(kButtonDepth)', 'flash::Text(name), flash::Text(depth)',title)
 text=once(text,'uint32_t clip = Member(ctx, holder, kButtonName);','uint32_t clip = Member(ctx, holder, name);',title)
 text=once(text,'bool JoinRing(X86Context& ctx, const TitleMenu& title, uint32_t root, uint32_t button) {',
 '''void PositionCoop(uint32_t holder, uint32_t lead, uint32_t coop, uint32_t quit, const TitleMenu& title, X86Context& ctx) {
    Follow(coop, lead);
    uint32_t above_quit = coop;
    if (std::strcmp(title.last, "optionsButton") != 0) {
        uint32_t profile = Member(ctx, holder, title.last_clip);
        if (flash::IsClip(profile)) { Follow(profile, coop); above_quit = profile; }
    }
    Follow(quit, above_quit);
}

bool JoinRing(X86Context& ctx, const TitleMenu& title, uint32_t root, uint32_t button, uint32_t coop) {''',title)
 text=once(text,'if (!first || !last || !menu || !flash::Button(ctx, button, first)) return false;',
 '''uint32_t options = Member(ctx, root, "optionsButton");
    if (!first || !last || !options || !menu || !flash::Button(ctx, button, first) ||
        !flash::Button(ctx, coop, first)) return false;
    if (!flash::Set(ctx, coop, "actionSelectPage", flash::String(ctx, "OptionsMenuXbox"))) return false;''',title)
 text=once(text,'    flash::Link(ctx, button, first);','''    flash::Link(ctx, button, first);
    flash::Link(ctx, options, coop);
    flash::Link(ctx, coop, last == options ? button : last);''',title)
 text=once(text,'    uint32_t button = CopyButton(ctx, title, holder);',
 '''    uint32_t button = Member(ctx, holder, kButtonName);
    if (!button) button = CopyButton(ctx, title, holder, kButtonName, kButtonDepth);
    uint32_t coop = Member(ctx, holder, kCoopButton);
    if (!coop) coop = CopyButton(ctx, title, holder, kCoopButton, "101");
    if (!coop || !game::CoopTitleLabel(ctx, coop)) return "co-op label";''',title)
 text=once(text,'    Follow(button, lead);\n    CallGuest', '    PositionCoop(holder, lead, coop, button, title, ctx);\n    CallGuest',title)
 text=once(text,'return JoinRing(ctx, title, root, button) ? nullptr : "ring";',
 'return JoinRing(ctx, title, root, button, coop) ? nullptr : "ring";',title)
 text=once(text,'    uint32_t lead = Member(ctx, holder, title.last_clip);','    uint32_t lead = Member(ctx, holder, "options_text");',title)
 text=once(text,'''    if (uint32_t button = Member(ctx, holder, kButtonName)) {
        Follow(button, lead);
        return;
    }''', '''    uint32_t button = Member(ctx, holder, kButtonName);
    uint32_t coop = Member(ctx, holder, kCoopButton);
    if (button && coop && root == g_buttons_root && button == g_buttons_quit && coop == g_buttons_coop) {
        PositionCoop(holder, lead, coop, button, title, ctx);
        if (!R32(ui + kPageLeaving) && R32(ui + kPageAsked) != 1 && InputTaken(ui) &&
            R32(ui + kMenuInput) == kInputConfirm &&
            Member(ctx, Member(ctx, root, "MyMenu"), "mFocusButton") == coop)
            game::RequestCoopRoom(std::strcmp(title.page, "blacktitlemenu2") == 0);
        return;
    }''',title)
 text=once(text,'        LOGI("quit: button added to %s", title.page);', '''        g_buttons_root = root;
        g_buttons_quit = Member(ctx, holder, kButtonName);
        g_buttons_coop = Member(ctx, holder, kCoopButton);
        LOGI("co-op UI: title entry added to %s; host/join unavailable", title.page);''',title)
 text=once(text,'    if (R32(flash::kPlayerReady) != 1 || !R32(flash::kPlayer) || R32(ui + kPageReady) != kPageRunning) return;',
 '''    if (R32(flash::kPlayerReady) != 1 || !R32(flash::kPlayer) || R32(ui + kPageReady) != kPageRunning) {
        g_buttons_root = g_buttons_quit = g_buttons_coop = 0;
        return;
    }''',title)
 pending[title]=text
 menu='src/game/options_menu.cpp';text=pending[menu]
 text=once(text,'#include "game/flash.h"','#include "game/flash.h"\n#include "coop/room_ui.h"\n#include "coop/room_route.hpp"',menu)
 text=once(text,'    kFields = kScroll + 2','    kCoopTitle = kScroll + 2,\n    kFields',menu)
 text=once(text,'    bool failed = false;','    bool failed = false;\n    const char* return_title = nullptr;',menu)
 text=once(text,'Page g_page;     // game thread','blackcoop::RoomRoute g_room_route;\nPage g_page;     // game thread',menu)
 text=once(text,'    p.mode = g_next;\n    g_next = 0;', '''    const auto direct = std::strcmp(host.page, "OptionsMenuXbox") == 0
        ? g_room_route.Take(blackcoop::Page::Options) : g_room_route.Take(blackcoop::Page::PauseOptions);
    p.return_title = direct == blackcoop::Title::Mission ? "blacktitlemenu2" :
                     direct == blackcoop::Title::Profile ? "blacktitlemenu" : nullptr;
    p.mode = p.return_title ? kEntries : g_next;
    g_next = 0;''',menu)
 text=once(text,'    const char* shown = p.mode ? kCaptions[p.mode - 1] : "buttons";',
 '''    if (!p.failed && p.return_title) {
        p.failed = !Set(ctx, menu, "mfuncFunctionBack", 0) ||
                   !Set(ctx, menu, "mstrPageBack", String(ctx, p.return_title));
    }
    const char* shown = p.mode ? kCaptions[p.mode - 1] : "buttons";''',menu)
 text=once(text,'    const Host* host = FindHost(name);', '''    const Host* host = FindHost(name);
    auto page = blackcoop::Page::Other;
    if (strncasecmp(name, "blacktitlemenu2", 15) == 0) page = blackcoop::Page::MissionTitle;
    else if (strncasecmp(name, "blacktitlemenu", 14) == 0) page = blackcoop::Page::ProfileTitle;
    else if (std::strcmp(name, "OptionsMenuXbox") == 0) page = blackcoop::Page::Options;
    const bool to_options = (R32(ui + kPageLeaving) || R32(ui + kPageAsked) == 1) &&
        strcasecmp(static_cast<const char*>(HostPtr(ui + kTargetPage)), "OptionsMenuXbox") == 0;
    // An intermediate unloaded page has no name; keep the token only through that load.
    if (*name) g_room_route.Observe(page, to_options);''',menu)
 text=once(text,'namespace game {','''namespace game {
bool CoopTitleLabel(X86Context& ctx, uint32_t clip) {
    return Show(ctx, kCoopTitle, clip, nullptr, "CO-OP");
}
void RequestCoopRoom(bool mission_title) {
    g_room_route.Request(mission_title ? blackcoop::Title::Mission : blackcoop::Title::Profile);
}''',menu)
 pending[menu]=text
 if len(pending)!=18:raise ValueError('Unexpected room UI output set')
 for name in pending:
  if name not in originals and (root/name).exists():raise ValueError('Refusing preexisting addition: '+name)
 for name,text in pending.items():
  p=root/name;p.parent.mkdir(parents=True,exist_ok=True);p.write_text(text,encoding='utf-8',newline='\n')
 return {name:hashlib.sha256((root/name).read_bytes()).hexdigest() for name in pending}
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('root',type=pathlib.Path)
 a=p.parse_args();print(json.dumps({'package':PACKAGE,'changed_files':prepare(a.root)},indent=2))

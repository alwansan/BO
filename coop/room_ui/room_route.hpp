#pragma once
namespace blackcoop {
enum class Title { None, Mission, Profile };
enum class Page { MissionTitle, ProfileTitle, Options, PauseOptions, Other };
// Game/UI thread only. A failed/cancelled transition cannot poison a later Options visit.
class RoomRoute {
    Title pending_ = Title::None;
public:
    void Request(Title title) { pending_ = title; }
    void Observe(Page page, bool transitioning_to_options) {
        if (page == Page::Options) return;
        const bool source = (pending_ == Title::Mission && page == Page::MissionTitle) ||
                            (pending_ == Title::Profile && page == Page::ProfileTitle);
        if (!source || !transitioning_to_options) pending_ = Title::None;
    }
    Title Take(Page page) {
        const Title result = page == Page::Options ? pending_ : Title::None;
        pending_ = Title::None;
        return result;
    }
};
}

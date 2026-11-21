#include "room_route.hpp"
#include <cassert>
int main() {
 using namespace blackcoop;
 RoomRoute route;
 assert(route.Take(Page::Options)==Title::None);
 route.Request(Title::Mission);
 route.Observe(Page::MissionTitle,true);
 assert(route.Take(Page::Options)==Title::Mission);
 assert(route.Take(Page::Options)==Title::None);
 route.Request(Title::Profile);
 route.Observe(Page::ProfileTitle,true);
 assert(route.Take(Page::PauseOptions)==Title::None);
 assert(route.Take(Page::Options)==Title::None);
 route.Request(Title::Mission);
 route.Observe(Page::MissionTitle,false);
 assert(route.Take(Page::Options)==Title::None);
 route.Request(Title::Mission);
 route.Observe(Page::Other,false);
 assert(route.Take(Page::Options)==Title::None);
 route.Request(Title::Profile);
 route.Observe(Page::Options,false);
 assert(route.Take(Page::Options)==Title::Profile);
 route.Request(Title::None);
 assert(route.Take(Page::Options)==Title::None);
}

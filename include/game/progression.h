#ifndef PROGRESSION_H
#define PROGRESSION_H

namespace completion {
constexpr const char* kIntroCompleteMarker = "marker.intro.complete";
constexpr const char* kQuestOriginsCompleteMarker = "marker.origins.complete";
constexpr const char* kQuestSpeakWithSeekerCompleteMarker =
    "marker.speak_with_seeker.complete";
// Speak with a seeker. Scattered around Mer.
constexpr const char* kQuestHollowSageMeetCompleteMarker =
    "marker.meet_hollow_sage.complete";
// Meet with the leader of the seekers - the hollow sage (doesn't speak)
constexpr const char* kQuestFightEmbraceCompleteMarker =
    "marker.fight_embrace.complete";
// Embrace is the bad guys in this game. (Like shadows in persona)
constexpr const char* kQuestFightWardenCompleteMarker =
    "marker.fight_warden.complete";
// Wardens are like the bosses.

}  // namespace completion

#endif  // PROGRESSION_H

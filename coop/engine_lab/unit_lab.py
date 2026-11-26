"""Validate the aggregate output of the private PC unit observer."""
import json

FIELDS = frozenset((
    "poll_calls", "unlink_calls", "link_calls", "before_frame_thread_calls", "other_thread_calls",
    "unit_publications", "ordinal_maps_verified", "ordinal_map_failures",
    "empty_publications", "unit_unlinks", "paired_unit_unlinks",
    "same_slot_unit_republications",
    "unit_link_transitions", "world_membership_verified", "world_membership_failures",
    "world_removal_verified", "world_removal_failures",
    "prop_tables_mapped", "prop_table_map_failures", "prop_records_mapped",
    "prop_world_membership_verified", "prop_world_membership_failures",
    "prop_world_removal_verified", "prop_world_removal_failures", "prop_removal_preconditions_missed",
))


def parse(text):
    def unique(pairs):
        result = dict(pairs)
        if len(result) != len(pairs):
            raise ValueError("duplicate unit field")
        return result

    if len(text) > 2048:
        raise ValueError("oversized unit aggregate")
    data = json.loads(text, object_pairs_hook=unique)
    if (not isinstance(data, dict) or set(data) != FIELDS or
            any(type(value) is not int or not 0 <= value <= 100000000 for value in data.values())):
        raise ValueError("invalid unit aggregate")
    if (data["unit_publications"] != data["ordinal_maps_verified"] +
            data["ordinal_map_failures"] + data["empty_publications"] or
            data["unit_publications"] > data["poll_calls"] or
            data["unit_unlinks"] > data["unlink_calls"] or
            data["paired_unit_unlinks"] > min(data["unit_unlinks"], data["ordinal_maps_verified"]) or
            data["same_slot_unit_republications"] > data["paired_unit_unlinks"] or
            data["unit_link_transitions"] > data["link_calls"] or
            data["world_membership_verified"] + data["world_membership_failures"] > data["unit_link_transitions"] or
            data["world_removal_verified"] + data["world_removal_failures"] > data["unit_unlinks"] or
            data["prop_tables_mapped"] + data["prop_table_map_failures"] > data["unit_link_transitions"] or
            data["prop_tables_mapped"] != data["prop_world_membership_verified"] +
            data["prop_world_membership_failures"] or
            not data["prop_tables_mapped"] <= data["prop_records_mapped"] <= 4096 * data["prop_tables_mapped"] or
            data["prop_world_removal_verified"] + data["prop_world_removal_failures"] > data["unit_unlinks"] or
            data["prop_removal_preconditions_missed"] > data["unlink_calls"]):
        raise ValueError("inconsistent unit observation counts")
    return data


def restart_lifecycle_observed(progress, units):
    """Require positive evidence and zero contradictions across the bounded run."""
    return (all(progress.get(key) is True for key in (
                'restart_armed', 'restart_gamecode_queued', 'restart_unload_removal',
                'restart_same_unit_relinked', 'restart_lifecycle_observed')) and
            units is not None and all(units[key] > 0 for key in (
                'ordinal_maps_verified', 'world_membership_verified', 'world_removal_verified')) and
            all(units[key] == 0 for key in ('other_thread_calls', 'ordinal_map_failures',
                'world_membership_failures', 'world_removal_failures')))

// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.hpp"

namespace
{  // UNNAMED NAMESPACE
// ==========================================================================
// Demon Hunter
// ==========================================================================

/* ==========================================================================
// Legion To-Do
// ==========================================================================

   General ------------------------------------------------------------------
   Soul Fragments from Shattered Souls
   Soul Fragment duration, travel time, positioning
     http://us.battle.net/wow/en/forum/topic/20743504316?page=14#280
   Demon Soul buff
   Fel Blade movement mechanics
   Darkness

   Havoc --------------------------------------------------------------------
   Demonic Appetite travel time
   Demonic Appetite fury from spell data
   Change Nemesis to be race specific instead of generic
   Nemesis buffs for each race?
   Defensive talent tier
   Second look at Momentum skills' timings
   Fury of the Illidari distance targeting support
   Overwhelming Power artifact trait
   Defensive artifact traits

   Vengeance ----------------------------------------------------------------
   Infernal Strike
   Torment
   Abyssal Strike, Last Resort, Nether Bond talents
   Infernal Force artifact trait
   Siphon Power artifact trait
     http://us.battle.net/wow/en/forum/topic/20743504316?page=15#282

   Needs Documenting --------------------------------------------------------
   Vengeful Retreat / Fel Rush "jump_cancel" option
   soul_fragments expressions
*/

/* Forward declarations
 */
class demon_hunter_t;
namespace actions
{
namespace attacks
{
struct melee_t;
struct chaos_blade_t;
}
}

/**
 * Demon Hunter target data
 * Contains target specific things
 */
class demon_hunter_td_t : public actor_target_data_t
{
public:
  struct dots_t
  {
    // Vengeance
    dot_t* fiery_brand;
  } dots;

  struct debuffs_t
  {
    // Havoc
    buff_t* anguish;
    buff_t* nemesis;

    // Vengeance
    buff_t* frail;
  } debuffs;

  demon_hunter_td_t( player_t* target, demon_hunter_t& p );
};

/* Demon Hunter class definition
 *
 * Derived from player_t. Contains everything that defines the Demon Hunter
 * class.
 */
class demon_hunter_t : public player_t
{
public:
  typedef player_t base_t;

  event_t* blade_dance_driver;
  std::vector<attack_t*> blade_dance_attacks;
  std::vector<attack_t*> death_sweep_attacks;

  // Autoattacks
  actions::attacks::melee_t* melee_main_hand;
  actions::attacks::melee_t* melee_off_hand;
  actions::attacks::chaos_blade_t* chaos_blade_main_hand;
  actions::attacks::chaos_blade_t* chaos_blade_off_hand;

  unsigned shear_counter; // # of failed Shears since last proc
  double metamorphosis_health; // Vengeance temp health from meta;

  // TODO: Do these expire?
  unsigned soul_fragments;
  unsigned lesser_soul_fragments;

  std::vector<cooldown_t*> sigil_cooldowns; // For Defiler's Lost Vambraces legendary

  // Buffs
  struct
  {
    // General
    buff_t* metamorphosis;

    // Havoc
    buff_t* blade_dance;
    buff_t* blur;
    buff_t* chaos_blades;
    buff_t* death_sweep;
    buff_t* momentum;
    buff_t* nemesis;
    buff_t* prepared;
    buff_t* rage_of_the_illidari;
    buff_t* vengeful_retreat_jump_cancel;

    // Vengeance
    buff_t* defensive_spikes;
    buff_t* demon_spikes;
    buff_t* empower_wards;
    buff_t* immolation_aura;
    buff_t* painbringer;
    absorb_buff_t* soul_barrier;
  } buff;

  // Talents
  struct
  {
    // General
    const spell_data_t* fel_eruption;

    const spell_data_t* felblade;
    
    // Havoc
    const spell_data_t* fel_mastery;
    const spell_data_t* demonic_appetite;
    const spell_data_t* blind_fury;

    const spell_data_t* prepared;
    const spell_data_t* demon_blades;

    const spell_data_t* first_blood;
    const spell_data_t* demon_reborn;

    const spell_data_t* momentum;
    const spell_data_t* bloodlet;
    const spell_data_t* nemesis;

    const spell_data_t* master_of_the_glaive;
    const spell_data_t* unleashed_power;
    const spell_data_t* chaos_cleave;

    const spell_data_t* chaos_blades;
    const spell_data_t* fel_barrage;
    const spell_data_t* demonic;

    // NYI
    const spell_data_t* netherwalk;
    const spell_data_t* desperate_instincts;

    // Vengeance
    const spell_data_t* blade_turning;
    const spell_data_t* fracture;

    const spell_data_t* agonizing_flames;
    const spell_data_t* quickened_sigils;
    
    const spell_data_t* soul_barrier;

    const spell_data_t* spirit_bomb;
    const spell_data_t* etched_in_blood;
    const spell_data_t* soul_rending;

    const spell_data_t* burning_alive;
    const spell_data_t* concentrated_sigils;
    const spell_data_t* feast_of_souls;

    const spell_data_t* fel_devastation;
    const spell_data_t* feed_the_demon;

    // NYI
    const spell_data_t* abyssal_strike;

    const spell_data_t* last_resort;

    const spell_data_t* brand_of_the_hunt;
    const spell_data_t* sigil_of_misery;
    const spell_data_t* razor_spikes;

    const spell_data_t* nether_bond;
  } talent;

  // Specialization Spells
  struct
  {
    // General
    const spell_data_t* demon_hunter;
    const spell_data_t* consume_magic;
    const spell_data_t* consume_soul;
    const spell_data_t* consume_soul_lesser;
    const spell_data_t* critical_strikes;
    const spell_data_t* leather_specialization;
    const spell_data_t* metamorphosis_buff;

    // Havoc
    const spell_data_t* havoc;
    const spell_data_t* annihilation;
    const spell_data_t* blade_dance;
    const spell_data_t* blur;
    const spell_data_t* chaos_nova;
    const spell_data_t* chaos_strike;
    const spell_data_t* death_sweep;
    const spell_data_t* demonic_appetite_fury;
    const spell_data_t* fel_barrage_proc;

    // Vengeance
    const spell_data_t* vengeance;
    const spell_data_t* demonic_wards;
    const spell_data_t* fiery_brand_dr;
    const spell_data_t* immolation_aura;
    const spell_data_t* soul_cleave;
    const spell_data_t* riposte;
    const spell_data_t* fragment_of_the_betrayers_prison;
  } spec;

  // Mastery Spells
  struct
  {
    const spell_data_t* demonic_presence; // Havoc
    const spell_data_t* fel_blood; // Vengeance
  } mastery_spell;

  // Artifacts
  struct artifact_spell_data_t
  {
    // Havoc -- Twinblades of the Deceiver
    artifact_power_t anguish_of_the_deceiver;
    artifact_power_t balanced_blades;
    artifact_power_t chaos_vision;
    artifact_power_t contained_fury;
    artifact_power_t critical_chaos;
    artifact_power_t demon_rage;
    artifact_power_t demon_speed;
    artifact_power_t feast_on_the_souls;
    artifact_power_t fury_of_the_illidari;
    artifact_power_t inner_demons;
    artifact_power_t rage_of_the_illidari;
    artifact_power_t sharpened_glaives;
    artifact_power_t unleashed_demons;
    artifact_power_t warglaives_of_chaos;

    // NYI
    artifact_power_t deceivers_fury;
    artifact_power_t illidari_knowledge;
    artifact_power_t overwhelming_power;

    // Vengeance -- The Aldrachi Warblades
    artifact_power_t aldrachi_design;
    artifact_power_t aura_of_pain;
    artifact_power_t charred_warblades;
    artifact_power_t defensive_spikes;
    artifact_power_t demonic_flames;
    artifact_power_t devour_souls;
    artifact_power_t embrace_the_pain;
    artifact_power_t fiery_demise;
    artifact_power_t fueled_by_pain;
    artifact_power_t honed_warblades;
    artifact_power_t painbringer;
    artifact_power_t shatter_the_souls;
    artifact_power_t soul_carver;
    artifact_power_t tormented_souls;
    artifact_power_t will_of_the_illidari;

    // NYI
    artifact_power_t infernal_force;
    artifact_power_t siphon_power;
  } artifact;

  // Cooldowns
  struct
  {
    // General
    cooldown_t* consume_magic;
    cooldown_t* felblade;
    cooldown_t* fel_eruption;

    // Havoc
    cooldown_t* blade_dance;
    cooldown_t* blur;
    cooldown_t* death_sweep;
    cooldown_t* chaos_nova;
    cooldown_t* chaos_blades;
    cooldown_t* demonic_appetite;
    cooldown_t* eye_beam;
    cooldown_t* fel_barrage;
    cooldown_t* fel_barrage_proc;
    cooldown_t* fel_rush;
    cooldown_t* fury_of_the_illidari;
    cooldown_t* nemesis;
    cooldown_t* netherwalk;
    cooldown_t* throw_glaive;
    cooldown_t* vengeful_retreat;

    // Vengeance
    cooldown_t* demon_spikes;
    cooldown_t* fiery_brand;
    cooldown_t* sigil_of_chains;
    cooldown_t* sigil_of_flame;
    cooldown_t* sigil_of_misery;
    cooldown_t* sigil_of_silence;
  } cooldown;

  // Gains
  struct
  {
    // General
    gain_t* miss_refund;

    // Havoc
    gain_t* demonic_appetite;
    gain_t* prepared;

    // Vengeance
    gain_t* blade_turning;
    gain_t* immolation_aura;
    gain_t* metamorphosis;
  } gain;

  // Benefits
  struct
  {
  } benefits;

  // Procs
  struct
  {
    // General
    proc_t* delayed_aa_range;
    proc_t* delayed_aa_channel;
    proc_t* soul_fragment;
    proc_t* soul_fragment_lesser;
    
    // Havoc
    proc_t* demonic_appetite;
    proc_t* demons_bite_in_meta;
    proc_t* felblade_reset;
    proc_t* fel_barrage;

    // Vengeance
    proc_t* fueled_by_pain;
  } proc;

  // RPPM objects
  struct rppms_t
  {
    // General
    real_ppm_t* felblade;

    // Havoc
    real_ppm_t* demon_blades;
    real_ppm_t* inner_demons;

    // Vengeance
    real_ppm_t* fueled_by_pain;
  } rppm;

  // Special
  struct
  {
    // General
    heal_t*   consume_soul;
    heal_t*   consume_soul_lesser;

    // Havoc
    spell_t*  anguish;
    attack_t* demon_blades;
    spell_t*  inner_demons;

    // Vengeance
    heal_t*   charred_warblades;
    spell_t*  immolation_aura;
    absorb_t* soul_barrier;
  } active;

  // Pets
  struct
  {
  } pets;

  // Options
  struct demon_hunter_options_t
  {
  } options;

  // Glyphs
  struct
  {
  } glyphs;

  // Legion Legendaries
  struct
  {
    // General
    const special_effect_t* moarg_bionic_stabilizers;

    // Havoc
    const special_effect_t* anger_of_the_halfgiants;
    const special_effect_t* eternal_hunger;
    const special_effect_t* loramus_thalipedes_sacrifice;
    const special_effect_t* raddons_cascading_eyes;

    // Vengeance
    const special_effect_t* cloak_of_fel_flames;
    const special_effect_t* fragment_of_the_betrayers_prison;
    const special_effect_t* ph_immolation_aura_damage_lowers_cd_of_fiery_brand;
    const special_effect_t* runemasters_pauldrons;
    const special_effect_t* the_defilers_lost_vambraces;
  } legendary;

  demon_hunter_t( sim_t* sim, const std::string& name, race_e r );

  // overridden player_t init functions
  stat_e  convert_hybrid_stat( stat_e s ) const override;
  void    copy_from( player_t* source ) override;
  action_t* create_action( const std::string& name, const std::string& options ) override;
  void    create_buffs() override;
  expr_t* create_expression( action_t*, const std::string& ) override;
  pet_t*  create_pet( const std::string& name, const std::string& type = std::string() ) override;
  std::string create_profile( save_e = SAVE_ALL ) override;
  bool    has_t18_class_trinket() const override;
  void    init_absorb_priority() override;
  void    init_action_list() override;
  void    init_base_stats() override;
  void    init_procs() override;
  void    init_resources( bool force ) override;
  void    init_rng() override;
  void    init_scaling() override;
  void    init_spells() override;
  void    invalidate_cache( cache_e ) override;
  resource_e primary_resource() const override;
  role_e  primary_role() const override;

  // custom demon_hunter_t init functions
private:
  void   apl_precombat();
  void   apl_default();
  void   apl_havoc();
  void   apl_vengeance();
  void   create_cooldowns();
  void   create_gains();
  void   create_benefits();
public:

  // overridden player_t stat functions
  double  composite_armor_multiplier() const override;
  double  composite_attack_power_multiplier() const override;
  double  composite_attribute_multiplier( attribute_e attr ) const override;
  double  composite_crit_avoidance() const override;
  double  composite_dodge() const override;
  double  composite_leech() const override;
  double  composite_melee_crit() const override;
  double  composite_melee_expertise( const weapon_t* ) const override;
  double  composite_parry() const override;
  double  composite_parry_rating() const override;
  double  composite_player_multiplier( school_e ) const override;
  double  composite_spell_crit() const override;
  double  matching_gear_multiplier( attribute_e attr ) const override;
  double  passive_movement_modifier() const override;
  double  temporary_movement_modifier() const override;

  // overridden player_t combat functions
  void   assess_damage( school_e, dmg_e, action_state_t* s ) override;
  demon_hunter_td_t* get_target_data( player_t* target ) const override;
  void   interrupt() override;
  void   recalculate_resource_max( resource_e ) override;
  void   reset() override;
  void   target_mitigation( school_e, dmg_e, action_state_t* ) override;
  
  // custom demon_hunter_t functions
  bool   consume_soul_fragments();
  double get_soul_fragments();
  void   spawn_soul_fragment( unsigned = 1 );
  void   spawn_soul_fragment_lesser( unsigned = 1 );

private:
  target_specific_t<demon_hunter_td_t> _target_data;
};

namespace pets
{
// ==========================================================================
// Demon Hunter Pets
// ==========================================================================

/* Demon Hunter pet base
 *
 * defines characteristics common to ALL Demon Hunter pets
 */
struct demon_hunter_pet_t : public pet_t
{
  demon_hunter_pet_t( sim_t* sim, demon_hunter_t& owner,
                      const std::string& pet_name, pet_e pt,
                      bool guardian = false )
    : pet_t( sim, &owner, pet_name, pt, guardian )
  {
    base.position = POSITION_BACK;
    base.distance = 3;
  }

  struct _stat_list_t
  {
    int level;
    std::array<double, ATTRIBUTE_MAX> stats;
  };

  void init_base_stats() override
  {
    pet_t::init_base_stats();

    owner_coeff.ap_from_sp = 1.0;
    owner_coeff.sp_from_sp = 1.0;

    // Base Stats, same for all pets. Depends on level
    static const _stat_list_t pet_base_stats[] = {
      //   none, str, agi, sta, int, spi
      {85, {{0, 453, 883, 353, 159, 225}}},
    };

    // Loop from end to beginning to get the data for the highest available
    // level equal or lower than the player level
    int i = as<int>( sizeof_array( pet_base_stats ) );
    while ( --i > 0 )
    {
      if ( pet_base_stats[ i ].level <= level() )
        break;
    }
    if ( i >= 0 )
      base.stats.attribute = pet_base_stats[ i ].stats;
  }

  void schedule_ready( timespan_t delta_time, bool waiting ) override
  {
    if ( main_hand_attack && !main_hand_attack -> execute_event )
    {
      main_hand_attack -> schedule_execute();
    }

    pet_t::schedule_ready( delta_time, waiting );
  }

  double composite_player_multiplier( school_e school ) const override
  {
    double m = pet_t::composite_player_multiplier( school );

    // Orc racial
    m *= 1.0 + o().racials.command -> effectN( 1 ).percent();

    return m;
  }

  resource_e primary_resource() const override
  {
    return RESOURCE_ENERGY;
  }

  demon_hunter_t& o() const
  {
    return static_cast<demon_hunter_t&>( *owner );
  }
};

namespace actions
{  // namespace for pet actions

}  // end namespace actions ( for pets )

}  // END pets NAMESPACE

struct delayed_execute_event_t : public event_t
{
  action_t* action;
  player_t* target;

  delayed_execute_event_t( demon_hunter_t* p, action_t* a, player_t* t,
                        timespan_t delay )
    : event_t( *p -> sim ), action( a ), target( t )
  {
    add_event( delay );

    assert( action -> background );
  }

  const char* name() const override
  {
    return "Delayed Execute";
  }

  void execute() override
  {
    if ( ! target -> is_sleeping() )
    {
      action -> target = target;
      action -> schedule_execute();
    }
  }
};

namespace actions
{
/* This is a template for common code for all Demon Hunter actions.
 * The template is instantiated with either spell_t, heal_t or absorb_t as the
 * 'Base' class.
 * Make sure you keep the inheritance hierarchy and use base_t in the derived
 * class,
 * don't skip it and call spell_t/heal_t or absorb_t directly.
 */
template <typename Base>
class demon_hunter_action_t : public Base
{
public:
  gain_t* action_gain;
  bool metamorphosis_gcd;
  bool hasted_gcd, hasted_cd;
  bool demonic_presence;

  demon_hunter_action_t( const std::string& n, demon_hunter_t* p,
                         const spell_data_t* s = spell_data_t::nil() )
    : ab( n, p, s ),
      action_gain( p -> get_gain( n ) ),
      metamorphosis_gcd( p -> specialization() == DEMON_HUNTER_HAVOC &&
        ab::data().affected_by( p -> spec.metamorphosis_buff -> effectN( 7 ) ) ),
      hasted_gcd( false ),
      hasted_cd( false ),
      demonic_presence( ab::data().affected_by(
        p -> mastery_spell.demonic_presence -> effectN( 1 ) ) )
  {
    ab::may_crit      = true;
    ab::tick_may_crit = true;

    switch ( p -> specialization() )
    {
      case DEMON_HUNTER_HAVOC:
        hasted_gcd = ab::data().affected_by( p -> spec.havoc -> effectN( 1 ) );
        ab::cooldown -> hasted =
          ab::data().affected_by( p -> spec.havoc -> effectN( 2 ) );
        break;
      case DEMON_HUNTER_VENGEANCE:
        hasted_gcd = ab::data().affected_by( p -> spec.vengeance -> effectN( 1 ) );
        ab::cooldown -> hasted =
          ab::data().affected_by( p -> spec.vengeance -> effectN( 2 ) );
        break;
      default:
        break;
    }
  }

  demon_hunter_t* p()
  { return static_cast<demon_hunter_t*>( ab::player ); }

  const demon_hunter_t* p() const
  { return static_cast<const demon_hunter_t*>( ab::player ); }

  demon_hunter_td_t* td( player_t* t ) const
  { return p() -> get_target_data( t ); }

  void init() override
  {
    ab::init();

    if ( p() -> specialization() == DEMON_HUNTER_HAVOC )
    {
      if ( ( ab::weapon && ab::weapon_multiplier ) || ab::attack_power_mod.direct ||
        ab::attack_power_mod.tick )
      {
        if ( dbc::is_school( ab::school, SCHOOL_CHAOS ) + demonic_presence == 1 )
        {
          if ( p() -> bugs )
            ab::sim -> errorf( "%s (%u) school and %s benefit do not match!",
              ab::name_str.c_str(), ab::data().id(),
              p() -> mastery_spell.demonic_presence -> name_cstr() );
          else
            demonic_presence = ! demonic_presence;
        }
      }

      if ( ab::trigger_gcd >= timespan_t::from_seconds( 1.0 ) && ! metamorphosis_gcd )
      {
        if ( p() -> bugs )
          ab::sim -> errorf( "%s (%u) does not benefit from metamorphosis!",
            ab::name_str.c_str(), ab::data().id() );
        else
          metamorphosis_gcd = true;
      }
    }
  }

  timespan_t gcd() const override
  {
    timespan_t g = ab::gcd();

    if ( g == timespan_t::zero() )
      return g;

    if ( metamorphosis_gcd && p() -> buff.metamorphosis -> check() )
    {
      g += p() -> spec.metamorphosis_buff -> effectN( 7 ).time_value();
    }

    if ( hasted_gcd )
    {
      g *= p() -> cache.attack_haste();
    }

    if ( g < ab::min_gcd )
      g = ab::min_gcd;

    return g;
  }

  virtual double action_multiplier() const override
  {
    double am = ab::action_multiplier();

    if ( demonic_presence )
    {
      am *= 1.0 + p() -> cache.mastery_value();
    }

    return am;
  }

  virtual double composite_target_multiplier( player_t* t ) const override
  {
    double tm = ab::composite_target_multiplier( t );

    tm *= 1.0 + td( t ) -> debuffs.nemesis -> current_value;

    if ( dbc::is_school( ab::school, SCHOOL_FIRE ) )
    {
      tm *= 1.0 + td( t ) -> dots.fiery_brand -> is_ticking() *
        p() -> artifact.fiery_demise.percent();
    }

    return tm;
  }

  virtual void consume_resource() override
  {
    ab::consume_resource();

    if ( p() -> talent.etched_in_blood -> ok() && ab::resource_current == RESOURCE_PAIN &&
      ab::resource_consumed > 0 )
    {
      // No spell data values... like at all.
      timespan_t reduction = ( ab::resource_consumed / 20.0 ) * timespan_t::from_seconds( -1.0 );

      p() -> cooldown.sigil_of_flame -> adjust( reduction );
    }
  }

  virtual void tick( dot_t* d ) override
  {
    ab::tick( d );

    trigger_charred_warblades( d -> state );
  }

  virtual void impact( action_state_t* s ) override
  {
    ab::impact( s );

    if ( ab::result_is_hit( s -> result ) )
    {
      trigger_charred_warblades( s );

      // Benefit tracking
      p() -> get_target_data( s -> target ) -> debuffs.nemesis -> up();
    }
  }

  virtual void execute() override
  {
    ab::execute();

    if ( ! ab::hit_any_target && ab::resource_consumed > 0 )
    {
      trigger_refund();
    }
  }

  virtual bool usable_moving() const override
  {
    if ( ab::execute_time() > timespan_t::zero() )
      return false;

    if ( p() -> buff.vengeful_retreat_jump_cancel -> check() )
      return true;

    return ab::usable_moving();
  }

  void trigger_refund()
  {
    if ( ab::resource_current == RESOURCE_FURY || ab::resource_current == RESOURCE_PAIN )
    {
      p() -> resource_gain( ab::resource_current, ab::resource_consumed * 0.80,
        p() -> gain.miss_refund );
    }
  }

  void trigger_charred_warblades( action_state_t* s )
  {
    if ( ! p() -> artifact.charred_warblades.rank() )
    {
      return;
    }

    if ( ! dbc::is_school( ab::school, SCHOOL_FIRE ) )
    {
      return;
    }

    if ( ! ( ab::harmful && s -> result_amount > 0 ) )
    {
      return;
    }

    heal_t* action = p() -> active.charred_warblades;
    action -> base_dd_min = action -> base_dd_max = s -> result_amount;
    action -> schedule_execute();
  }

protected:
  /// typedef for demon_hunter_action_t<action_base_t>
  typedef demon_hunter_action_t base_t;

private:
  /// typedef for the templated action type, eg. spell_t, attack_t, heal_t
  typedef Base ab;
};

// ==========================================================================
// Demon Hunter heals
// ==========================================================================

struct demon_hunter_heal_t : public demon_hunter_action_t<heal_t>
{
  demon_hunter_heal_t( const std::string& n, demon_hunter_t* p,
                       const spell_data_t* s = spell_data_t::nil() )
    : base_t( n, p, s )
  {
    harmful = false;
    target = p;
  }
};

namespace heals
{

// Consume Soul =============================================================

struct consume_soul_t : public demon_hunter_heal_t
{
  struct soul_barrier_t : public demon_hunter_action_t<absorb_t>
  {
    double maximum_absorb;

    soul_barrier_t( demon_hunter_t* p ) :
      demon_hunter_action_t<absorb_t>( "soul_barrier", p, p -> find_spell( 211512 ) )
    {
      background = true;
      may_crit = false;
      maximum_absorb = attack_power_mod.direct * 10; // May 1 2016
    }

    void assess_damage( dmg_e type, action_state_t* s ) override
    {
      // Stacking absorb logic.
      s -> result_amount += p() -> buff.soul_barrier -> check_value();
      s -> result_amount = std::min( s -> result_amount, maximum_absorb * s -> attack_power );
      
      demon_hunter_action_t<absorb_t>::assess_damage( type, s );
    }
  };

  unsigned* soul_counter;

  consume_soul_t( demon_hunter_t* p, const std::string& n,
      const spell_data_t* s, unsigned* souls ) :
    demon_hunter_heal_t( n, p, s ), soul_counter( souls )
  {
    background = true;

    if ( p -> talent.demonic_appetite -> ok() ) 
    {
      energize_type = ENERGIZE_ON_CAST;
      energize_resource = RESOURCE_FURY;
      energize_amount = p -> spec.demonic_appetite_fury -> effectN( 1 ).resource( RESOURCE_FURY );
      gain = p -> gain.demonic_appetite;
    }

    if ( p -> talent.soul_barrier -> ok() && ! p -> active.soul_barrier )
    {
      p -> active.soul_barrier = new soul_barrier_t( p );
    }
  }

  void execute() override
  {
    assert( *soul_counter > 0 );

    bool trigger_soul_barrier =
      p() -> talent.soul_barrier -> ok() && p() -> health_percentage() == 100;

    demon_hunter_heal_t::execute();

    ( *soul_counter )--;

    // Feast on the Souls only procs from major soul fragments.
    if ( p() -> artifact.feast_on_the_souls.rank() && soul_counter == &( p() -> soul_fragments ) )
    {
      timespan_t t = -p() -> artifact.feast_on_the_souls.time_value();

      p() -> cooldown.eye_beam -> adjust( t );
      p() -> cooldown.chaos_nova -> adjust( t );
    }

    if ( trigger_soul_barrier )
    {
      p() -> active.soul_barrier -> schedule_execute();
    }

    if ( p() -> talent.feed_the_demon -> ok() )
    {
      // Not in spell data.
      p() -> cooldown.demon_spikes -> adjust( timespan_t::from_seconds( -1.0 ) );
    }

    if ( p() -> artifact.fueled_by_pain.rank() && p() -> rppm.fueled_by_pain -> trigger() )
    {
      p() -> proc.fueled_by_pain -> occur();

      timespan_t duration = timespan_t::from_seconds(
        p() -> artifact.fueled_by_pain.data().effectN( 1 ).base_value() );

      p() -> buff.metamorphosis -> trigger(
        1, p() -> buff.metamorphosis -> default_value, -1.0, duration );
    }

    p() -> buff.painbringer -> trigger();
  }

  bool ready() override
  {
    if ( *soul_counter == 0 )
    {
      return false;
    }

    return demon_hunter_heal_t::ready();
  }
};

// Charred Warblades ========================================================

struct charred_warblades_t : public demon_hunter_heal_t
{
  charred_warblades_t( demon_hunter_t* p ) :
    demon_hunter_heal_t( "charred_warblades", p, p -> find_spell( 213011 ) )
  {
    background = true;
    may_crit = false;
    base_multiplier *= p -> artifact.charred_warblades.data().effectN( 1 ).percent();
  }

  void init() override
  {
    demon_hunter_heal_t::init();

    snapshot_flags &= ~STATE_VERSATILITY; // Not affected by Versatility.
  }
};

// Fel Devastation ==========================================================

struct fel_devastation_heal_t : public demon_hunter_heal_t
{
  fel_devastation_heal_t( demon_hunter_t* p ) :
    demon_hunter_heal_t( "fel_devastation_heal", p, p -> find_spell( 212106 ) )
  {
    background = true;
    may_crit = false;
  }
};

// Soul Cleave ==============================================================

struct soul_cleave_heal_t : public demon_hunter_heal_t
{
  double pct_heal;

  soul_cleave_heal_t( demon_hunter_t* p ) :
    demon_hunter_heal_t( "soul_cleave_heal", p, p -> spec.soul_cleave ),
    pct_heal( 0.0 )
  {
    background = true;
    base_costs.fill( 0 ); // This action is free; the parent pays the cost.

    if ( p -> talent.feast_of_souls -> ok() )
    {
      const spell_data_t* s = p -> find_spell( 207693 );
      dot_duration = s -> duration();
      base_tick_time = s -> effectN( 1 ).period();
      attack_power_mod.tick = 1.30; // Not in spell data.
      hasted_ticks = false;
    }

    pct_heal = p -> artifact.devour_souls.percent();
  }

  void execute() override
  {
    base_dd_min = base_dd_max = p() -> max_health() * pct_heal;

    demon_hunter_heal_t::execute();
  }
};

}

// ==========================================================================
// Demon Hunter spells
// ==========================================================================

struct demon_hunter_spell_t : public demon_hunter_action_t<spell_t>
{
  demon_hunter_spell_t( const std::string& n, demon_hunter_t* p,
                        const spell_data_t* s = spell_data_t::nil() )
    : base_t( n, p, s )
  {}
};

namespace spells
{

// Anguish ==================================================================

struct anguish_t : public demon_hunter_spell_t
{
  anguish_t( demon_hunter_t* p )
    : demon_hunter_spell_t( "anguish", p, p -> find_spell( 202446 ) )
  {
    background = dual = true;
    base_dd_min = base_dd_max = 0;
  }
};

// Blur =====================================================================

struct blur_t : public demon_hunter_spell_t
{
  blur_t( demon_hunter_t* p, const std::string& options_str )
    : demon_hunter_spell_t( "blur", p, p -> spec.blur )
  {
    parse_options( options_str );

    may_miss = may_block = may_dodge = may_parry = may_crit = false;
  }

  void execute() override
  {
    demon_hunter_spell_t::execute();

    p() -> buff.blur -> trigger();

    if ( p() -> artifact.demon_speed.rank() )
    {
      p() -> cooldown.fel_rush -> reset( false );
      p() -> cooldown.fel_rush -> reset( false );
    }
  }
};

// Chaos Blades =============================================================

struct chaos_blades_t : public demon_hunter_spell_t
{
  chaos_blades_t( demon_hunter_t* p, const std::string& options_str )
    : demon_hunter_spell_t( "chaos_blades", p, p -> talent.chaos_blades )
  {
    parse_options( options_str );

    may_miss = may_block = may_dodge = may_parry = may_crit = false;
  }

  void execute() override
  {
    demon_hunter_spell_t::execute();

    p() -> buff.chaos_blades -> trigger();
  }
};

// Chaos Nova ===============================================================

struct chaos_nova_t : public demon_hunter_spell_t
{
  chaos_nova_t( demon_hunter_t* p, const std::string& options_str )
    : demon_hunter_spell_t( "chaos_nova", p, p -> spec.chaos_nova )
  {
    parse_options( options_str );
    
    aoe = -1;
    cooldown -> duration += p -> talent.unleashed_power -> effectN( 1 ).time_value();
    base_costs[ RESOURCE_FURY ] *= 1.0 + p -> talent.unleashed_power -> effectN( 2 ).percent();
    if ( ! p -> bugs )
    {
      school = SCHOOL_CHAOS;
    }
  }
};

// Consume Magic ============================================================

struct consume_magic_t : public demon_hunter_spell_t
{
  resource_e resource;
  double resource_amount;

  consume_magic_t( demon_hunter_t* p, const std::string& options_str )
    : demon_hunter_spell_t( "consume_magic", p, p -> spec.consume_magic )
  {
    parse_options( options_str );
    
    use_off_gcd = ignore_false_positive = true;
    may_miss = may_block = may_dodge = may_parry = may_crit = false;

    const spelleffect_data_t effect = p -> find_spell( 218903 ) -> effectN(
      p -> specialization() == DEMON_HUNTER_HAVOC ? 1 : 2 );
    resource        = effect.resource_gain_type();
    resource_amount = effect.resource( resource );
  }

  void execute() override
  {
    demon_hunter_spell_t::execute();

    p() -> resource_gain( resource, resource_amount, action_gain );
  }

  bool ready() override
  {
    if ( ! target -> debuffs.casting -> check() )
      return false;

    return demon_hunter_spell_t::ready();
  }
};

// Demon Spikes =============================================================

struct demon_spikes_t : public demon_hunter_spell_t
{
  demon_spikes_t( demon_hunter_t* p, const std::string& options_str ) :
    demon_hunter_spell_t( "demon_spikes", p,
      p -> find_specialization_spell( "Demon Spikes" ) )
  {
    parse_options( options_str );

    use_off_gcd = true;
  }

  void execute() override
  {
    demon_hunter_spell_t::execute();

    p() -> buff.demon_spikes -> trigger();
    p() -> buff.defensive_spikes -> trigger();
  }
};

// Empower Wards ===========================================================

struct empower_wards_t : public demon_hunter_spell_t
{
  empower_wards_t( demon_hunter_t* p, const std::string& options_str ) : 
    demon_hunter_spell_t( "empower_wards", p,
      p -> find_specialization_spell( "Empower Wards" ) )
  {
    parse_options( options_str );

    use_off_gcd = true;
    base_dd_min = base_dd_max = 0;
  }

  void execute() override
  {
    demon_hunter_spell_t::execute();

    p() -> buff.empower_wards -> trigger();
  }
};

// Eye Beam =================================================================

struct eye_beam_t : public demon_hunter_spell_t
{
  struct eye_beam_tick_t : public demon_hunter_spell_t
  {
    eye_beam_tick_t( demon_hunter_t* p )
      : demon_hunter_spell_t( "eye_beam_tick", p, p -> find_spell( 198030 ) )
    {
      aoe  = -1;
      dual = background = true;
      if ( ! p -> bugs )
      {
        school = SCHOOL_CHAOS;
      }

      base_multiplier *= 1.0 + p -> artifact.chaos_vision.percent();
    }

    dmg_e amount_type( const action_state_t*, bool ) const override
    { return DMG_OVER_TIME; }  // TOCHECK

    void impact( action_state_t* s ) override
    {
      demon_hunter_spell_t::impact( s );

      if ( result_is_hit( s -> result ) && p() -> artifact.anguish_of_the_deceiver.rank() )
      {
        td( s -> target ) -> debuffs.anguish -> trigger();
      }

      if ( p() -> legendary.raddons_cascading_eyes && s -> result == RESULT_CRIT )
      {
        timespan_t t = timespan_t::from_millis( p() -> legendary.raddons_cascading_eyes
          -> driver() -> effectN( 1 ).base_value() );
        p() -> cooldown.eye_beam -> adjust( -t );
      }
    }
  };

  eye_beam_tick_t* beam;

  eye_beam_t( demon_hunter_t* p, const std::string& options_str )
    : demon_hunter_spell_t( "eye_beam", p, p -> find_class_spell( "Eye Beam" ) ),
      beam( new eye_beam_tick_t( p ) )
  {
    parse_options( options_str );

    may_miss = may_crit = false;
    harmful = false; // Disables bleeding on the target.
    channeled = true;
    beam -> stats = stats;
    if ( ! p -> bugs )
    {
      school = SCHOOL_CHAOS;
    }

    dot_duration *= 1.0 + p -> talent.blind_fury -> effectN( 1 ).percent();

    if ( p -> artifact.anguish_of_the_deceiver.rank() )
    {
      add_child( p -> active.anguish );
    }
  }

  /* Don't record data for this action, since we don't want that 0
     damage hit incorporated into statistics. */
  virtual void record_data( action_state_t* ) override {}

  // Channel is not hasted.
  timespan_t tick_time( double ) const override
  { return base_tick_time; }

  void tick( dot_t* d ) override
  {
    demon_hunter_spell_t::tick( d );

    // Until 400ms through the channel, it's just animation and doesn't actually
    // deal damage.
    if ( d -> current_tick >= 2 )
    {
      beam -> schedule_execute();
    }
  }

  void execute() override
  {
    demon_hunter_spell_t::execute();

    if ( p() -> talent.demonic -> ok() )
    {
      timespan_t duration = timespan_t::from_seconds( 5.0 )
        + std::max( gcd(), composite_dot_duration( execute_state ) );
      p() -> buff.metamorphosis -> trigger(
        1, p() -> buff.metamorphosis -> default_value, -1.0, duration );
    }
  }
};

// Fel Barrage ==============================================================

struct fel_barrage_t : public demon_hunter_spell_t
{
  struct fel_barrage_tick_t : public demon_hunter_spell_t
  {
    fel_barrage_tick_t( demon_hunter_t* p ) :
      demon_hunter_spell_t( "fel_barrage_tick", p, p -> find_spell( 211052 ) )
    {
      background = dual = true;
      aoe = -1;
    }
  };

  fel_barrage_t( demon_hunter_t* p, const std::string& options_str ) :
    demon_hunter_spell_t( "fel_barrage", p, p -> talent.fel_barrage )
  {
    parse_options( options_str );

    tick_action = new fel_barrage_tick_t( p );
    channeled = tick_zero = true;
    dot_duration = data().duration();
    base_tick_time = data().effectN( 1 ).period();
  }

  // Hide direct results in report.
  virtual void record_data( action_state_t* ) override {}

  timespan_t travel_time() const override
  { return timespan_t::zero(); } 
  
  // Channel is not hasted. TOCHECK
  timespan_t tick_time( double ) const override
  { return base_tick_time; }
  
  double composite_persistent_multiplier( const action_state_t* s ) const override
  {
    double pm = demon_hunter_spell_t::composite_persistent_multiplier( s );

    pm *= ( double ) cooldown -> current_charge / cooldown -> charges;

    return pm;
  }

  void update_ready( timespan_t cd_duration ) override
  {
    assert( cooldown -> current_charge > 0 );

    /* A bit of a dirty hack to consume all charges. Just consume all but one
       and let action_t::update_ready() take it from there. */
    cooldown -> current_charge = 1;

    demon_hunter_spell_t::update_ready( cd_duration );
  }

  bool usable_moving() const override
  { return true; }
};

// Fel Devastation ==========================================================

struct fel_devastation_t : public demon_hunter_spell_t
{
  struct fel_devastation_tick_t : public demon_hunter_spell_t
  {
    fel_devastation_tick_t( demon_hunter_t* p ) :
      demon_hunter_spell_t( "fel_devastation_tick", p, p -> find_spell( 212105 ) )
    {
      aoe = -1;
      background = dual = true;
    }

    dmg_e amount_type( const action_state_t*, bool ) const override
    { return DMG_OVER_TIME; }  // TOCHECK
  };

  heals::fel_devastation_heal_t* heal;
  fel_devastation_tick_t* damage;

  fel_devastation_t( demon_hunter_t* p, const std::string& options_str ) :
    demon_hunter_spell_t( "fel_devastation", p, p -> talent.fel_devastation )
  {
    parse_options( options_str );

    channeled = true;
    may_miss = may_crit = false;
    harmful = false; // Disables bleeding on the target.
    heal = new heals::fel_devastation_heal_t( p );
    damage = new fel_devastation_tick_t( p );
    damage -> stats = stats;
  }

  // Don't record data for this action.
  void record_data( action_state_t* ) override {}

  // Channel is not hasted.
  timespan_t tick_time( double ) const override
  { return base_tick_time; }

  void tick( dot_t* d ) override
  {
    demon_hunter_spell_t::tick( d );

    // Heal happens first.
    heal -> schedule_execute();
    damage -> schedule_execute();
  }
};

// Fel Rush =================================================================

struct fel_rush_t : public demon_hunter_spell_t
{
  struct fel_rush_damage_t : public demon_hunter_spell_t
  {
    fel_rush_damage_t( demon_hunter_t* p ) :
      demon_hunter_spell_t( "fel_rush_dmg", p, p -> find_spell( 192611 ) )
    {
      aoe = -1;
      dual = background = true;
      may_miss = may_dodge = may_block = false;

      base_crit += p -> talent.fel_mastery -> effectN( 2 ).percent();

      if ( p -> talent.fel_mastery -> ok() )
      {
        energize_type = ENERGIZE_ON_HIT;
        energize_resource = RESOURCE_FURY;
        energize_amount = p -> talent.fel_mastery -> effectN( 1 ).resource( RESOURCE_FURY );
      }

      if ( p -> legendary.loramus_thalipedes_sacrifice )
      {
        base_add_multiplier *= 1.0 + p -> legendary.loramus_thalipedes_sacrifice
          -> driver() -> effectN( 1 ).percent();
      }
    }
  };

  bool jump_cancel;

  fel_rush_t( demon_hunter_t* p, const std::string& options_str ) :
    demon_hunter_spell_t( "fel_rush", p, p -> find_class_spell( "Fel Rush" ) ),
    jump_cancel( false )
  {
    add_option( opt_bool( "jump_cancel", jump_cancel ) );
    parse_options( options_str );

    may_block = may_crit = false;
    min_gcd = trigger_gcd;
    if ( ! jump_cancel )
    {
      base_teleport_distance  = p -> find_spell( 192611 ) -> effectN( 1 ).radius();
      movement_directionality = MOVEMENT_OMNI;
      ignore_false_positive   = true;
    }

    impact_action = new fel_rush_damage_t( p );
    impact_action -> stats = stats;

    // Add damage modifiers in fel_rush_damage_t, not here.
  }

  /* Don't record data for this action, since we don't want that 0
     damage hit incorporated into statistics. */
  virtual void record_data( action_state_t* ) override {}

  timespan_t gcd() const override
  {
    timespan_t g = demon_hunter_spell_t::gcd();
    
    // Fel Rush's loss of control causes a GCD lag after the loss ends.
    // TOCHECK: Does this delay happen when jump cancelling?
    g += rng().gauss( sim -> gcd_lag, sim -> gcd_lag_stddev );

    return g;
  }

  void execute() override
  {
    demon_hunter_spell_t::execute();

    p() -> buff.momentum -> trigger();

    if ( ! jump_cancel )
    {
      // Buff to track the rush's movement. This lets us delay autoattacks.
      p() -> buffs.self_movement -> trigger( 1, 0, -1.0, data().gcd() );

      // Adjust new distance from target.
      p() -> current.distance =
        std::abs( p() -> current.distance - composite_teleport_distance( execute_state ) );

      // If new distance after rushing is too far away to melee from, then trigger
      // movement back into melee range.
      if ( p() -> current.distance > 5.0 )
      {
        p() -> trigger_movement( p() -> current.distance - 5.0, MOVEMENT_TOWARDS );
      }
    }
  }

  bool ready() override
  {
    if ( p() -> buffs.self_movement -> check() )
      return false;

    return demon_hunter_spell_t::ready();
  }
};

// Fel Eruption =============================================================

struct fel_eruption_t : public demon_hunter_spell_t
{
  struct fel_eruption_damage_t : public demon_hunter_spell_t
  {
    fel_eruption_damage_t( demon_hunter_t* p ) :
      demon_hunter_spell_t( "fel_eruption_dmg", p, p -> find_spell( 225102 ) )
    {
      background = dual = true;
      may_miss = false;
      // Assume the target is stun immune.
      base_multiplier *= 1.0 + p -> talent.fel_eruption -> effectN( 1 ).percent(); 
      if ( ! p -> bugs )
      {
        school = SCHOOL_CHAOS;
      }
    }
  };

  fel_eruption_t( demon_hunter_t* p, const std::string& options_str ) :
    demon_hunter_spell_t( "fel_eruption", p, p -> talent.fel_eruption )
  {
    parse_options( options_str );

    may_crit = false;
    resource_current = p -> specialization() == DEMON_HUNTER_HAVOC ? RESOURCE_FURY : RESOURCE_PAIN;

    impact_action = new fel_eruption_damage_t( p );
    impact_action -> stats = stats;

    // Add damage modifiers in fel_eruption_damage_t, not here.
  }

  // Don't record data for this action.
  void record_data( action_state_t* ) override {}
};

// Fiery Brand ==============================================================

struct fiery_brand_t : public demon_hunter_spell_t
{
  struct fiery_brand_state_t : public action_state_t
  {
    bool primary;

    fiery_brand_state_t( action_t* a, player_t* target ) :
      action_state_t( a, target )
    {}

    void initialize() override
    {
      action_state_t::initialize();

      primary = false;
    }

    void copy_state( const action_state_t* s ) override
    {
      action_state_t::copy_state( s );

      primary = debug_cast<const fiery_brand_state_t*>( s ) -> primary;
    }

    std::ostringstream& debug_str( std::ostringstream& s ) override
    {
      action_state_t::debug_str( s );

      s << " primary=" << primary;

      return s;
    }
  };

  struct fiery_brand_dot_t : public demon_hunter_spell_t
  {
    fiery_brand_dot_t( demon_hunter_t* p ) :
      demon_hunter_spell_t( "fiery_brand_dot", p, p -> find_spell( 204022 ) )
    {
      background = dual = true;
      hasted_ticks = may_crit = false;
      school = p -> find_specialization_spell( "Fiery Brand" ) -> get_school_type();
      base_dd_min = base_dd_max = 0;

      dot_duration += p -> artifact.demonic_flames.time_value();

      if ( p -> talent.burning_alive -> ok() )
      {
        // Not in spell data.
        attack_power_mod.tick = 1.0;
        // Spread radius used for Burning Alive.
        radius = p -> find_spell( 207760 ) -> effectN( 1 ).radius();
      }
      else
      {
        /* If Burning alive isn't talented this isn't really a DoT, so let's
        turn off DoT callbacks and minimize the number of events generated.*/
        base_tick_time = dot_duration;
        tick_may_crit = callbacks = false;
      }
    }

    action_state_t* new_state() override
    { return new fiery_brand_state_t( this, target ); }

    virtual void record_data( action_state_t* s ) override
    {
      // Don't record data direct hits for this action.
      if ( s -> result_type != DMG_DIRECT )
      {
        demon_hunter_spell_t::record_data( s );
      }
    }

    void tick( dot_t* d ) override
    {
      demon_hunter_spell_t::tick( d );

      trigger_burning_alive( d );
    }

    void trigger_burning_alive( dot_t* d )
    {
      if ( ! p() -> talent.burning_alive -> ok() )
      {
        return;
      }

      if ( ! debug_cast<fiery_brand_state_t*>( d -> state ) -> primary )
      {
        return;
      }

      // Retrieve target list, checking for distance if necessary.
      std::vector<player_t*> targets = target_list();
      if ( sim -> distance_targeting_enabled )
      {
        targets = check_distance_targeting( targets );
      }

      if ( targets.size() == 1 )
      {
        return;
      }

      // Filter target list down to targets that are not already branded.
      std::vector<player_t*> candidates;

      for ( size_t i = 0; i < targets.size(); i++ )
      {
        if ( ! td( targets[ i ] ) -> dots.fiery_brand -> is_ticking() )
        {
          candidates.push_back( targets[ i ] );
        }
      }

      if ( candidates.size() == 0 )
      {
        return;
      }

      // Pick a random target.
      player_t* target =
        candidates[ ( int ) p() -> rng().range( 0, candidates.size() ) ];

      // Execute a dot on that target.
      this -> target = target;
      schedule_execute();
    }
  };

  fiery_brand_t( demon_hunter_t* p, const std::string& options_str ) :
    demon_hunter_spell_t( "fiery_brand", p,
      p -> find_specialization_spell( "Fiery Brand" ) )
  {
    use_off_gcd = true;
    impact_action = new fiery_brand_dot_t( p );
    impact_action -> stats = stats;
  }

  action_state_t* new_state() override
  { return new fiery_brand_state_t( this, target ); }

  void snapshot_state( action_state_t* s, dmg_e rt ) override
  {
    demon_hunter_spell_t::snapshot_state( s, rt );

    /* Set this DoT as the primary DoT, enabling its ticks to spread the DoT
       to nearby targets when Burning Alive is talented. impact_action will
       automatically copy this state to the dot state. */
    debug_cast<fiery_brand_state_t*>( s ) -> primary = true;
  }
};

// Immolation Aura ==========================================================

struct immolation_aura_damage_t : public demon_hunter_spell_t
{
  // TOCHECK: Direct, over time, or both?
  immolation_aura_damage_t( demon_hunter_t* p, spell_data_t* s ) : 
    demon_hunter_spell_t( "immolation_aura_dmg", p, s )
  {
    aoe = -1;
    background = dual = true;

    base_multiplier *= 1.0 + p -> talent.agonizing_flames -> effectN( 1 ).percent();\
    base_multiplier *= 1.0 + p -> artifact.aura_of_pain.percent();
  }

  void impact( action_state_t* s ) override
  {
    demon_hunter_spell_t::impact( s );

    if ( result_is_hit( s -> result ) )
    {
      if ( p() -> legendary.ph_immolation_aura_damage_lowers_cd_of_fiery_brand ) 
      {
        assert( p() -> legendary.ph_immolation_aura_damage_lowers_cd_of_fiery_brand
          -> driver() -> proc_chance() == 1.0 );

        if ( p() -> rng().roll( p() -> legendary.ph_immolation_aura_damage_lowers_cd_of_fiery_brand
          -> driver() -> effectN( 2 ).base_value() ) )
        {
          timespan_t t = timespan_t::from_seconds(
            p() -> legendary.ph_immolation_aura_damage_lowers_cd_of_fiery_brand
              -> driver() -> effectN( 1 ).base_value() );
          p() -> cooldown.fiery_brand -> adjust( -t );
        }
      }
    }
  }
};

struct immolation_aura_t : public demon_hunter_spell_t
{
  immolation_aura_damage_t* direct;

  immolation_aura_t( demon_hunter_t* p, const std::string& options_str ) : 
    demon_hunter_spell_t( "immolation_aura", p, p -> spec.immolation_aura )
  {
    parse_options( options_str );
    may_miss = may_crit = false;
    dot_duration = timespan_t::zero(); // damage is dealt by ticking buff

    direct = new immolation_aura_damage_t( p, data().effectN( 2 ).trigger() );
    direct -> stats = stats;

    // Add damage modifiers in immolation_aura_damage_t, not here.
  }

  /* Don't record data for this action, since we don't want that 0
     damage hit incorporated into statistics. */
  virtual void record_data( action_state_t* ) override {}

  void execute() override
  {
    p() -> buff.immolation_aura -> trigger();

    demon_hunter_spell_t::execute();

    p() -> resource_gain( RESOURCE_PAIN, data().effectN( 3 ).resource( RESOURCE_PAIN ),
      p() -> gain.immolation_aura );

    direct -> execute();
  }
};

// Inner Demons =============================================================

struct inner_demons_t : public demon_hunter_spell_t
{
  inner_demons_t( demon_hunter_t* p )
    : demon_hunter_spell_t( "inner_demons", p, p -> find_spell( 202388 ) )
  {
    background = true;
    aoe        = -1;
  }

  timespan_t travel_time() const override
  {
    return p() -> artifact.inner_demons.data().effectN( 1 ).trigger() -> duration();
  };
};

// Metamorphosis ============================================================

struct metamorphosis_t : public demon_hunter_spell_t
{
  struct metamorphosis_impact_t : public demon_hunter_spell_t
  {
    metamorphosis_impact_t( demon_hunter_t* p ) :
      demon_hunter_spell_t( "metamorphosis_impact", p, p -> find_spell( 200166 ) )
    {
      background = true;
      aoe = -1;
      dot_duration = timespan_t::zero();
      if ( ! p -> bugs )
      {
        school = SCHOOL_CHAOS;
      }
    }
  };

  metamorphosis_t( demon_hunter_t* p, const std::string& options_str ) :
    demon_hunter_spell_t( "metamorphosis", p,
                             p -> find_class_spell( "Metamorphosis" ) )
  {
    parse_options( options_str );

    may_miss = may_dodge = may_parry = may_crit = may_block = false;
    dot_duration = timespan_t::zero();
    base_teleport_distance  = data().max_range();
    movement_directionality = MOVEMENT_OMNI;
    impact_action = new metamorphosis_impact_t( p );

    cooldown -> duration += p -> artifact.unleashed_demons.time_value();
  }

  // leap travel time, independent of distance
  timespan_t travel_time() const override
  {
    return timespan_t::from_seconds( 1.0 );
  }

  void execute() override
  {
    demon_hunter_spell_t::execute();

    // Buff is gained at the start of the leap.
    p() -> buff.metamorphosis -> trigger();

    if ( p() -> talent.demon_reborn -> ok() )
    {
      p() -> cooldown.blade_dance -> reset( false );
      p() -> cooldown.blur -> reset( false );
      p() -> cooldown.chaos_blades -> reset( false );
      p() -> cooldown.chaos_nova -> reset( false );
      p() -> cooldown.consume_magic -> reset( false );
      p() -> cooldown.death_sweep -> reset( false );
      p() -> cooldown.eye_beam -> reset( false );
      p() -> cooldown.felblade -> reset( false );
      p() -> cooldown.fel_barrage -> reset( false );
      p() -> cooldown.fel_eruption -> reset( false );
      p() -> cooldown.fel_rush -> reset( false );
      p() -> cooldown.fel_rush -> reset( false );
      p() -> cooldown.nemesis -> reset( false );
      p() -> cooldown.netherwalk -> reset( false );
      p() -> cooldown.fury_of_the_illidari -> reset( false );
      p() -> cooldown.throw_glaive -> reset( false );
      if ( p() -> talent.master_of_the_glaive -> ok() )
      {
        p() -> cooldown.throw_glaive -> reset( false );
      }
      p() -> cooldown.vengeful_retreat -> reset( false );
    }

    if ( p() -> legendary.runemasters_pauldrons )
    {
      p() -> cooldown.sigil_of_chains -> reset( false );
      p() -> cooldown.sigil_of_flame -> reset( false );
      p() -> cooldown.sigil_of_misery -> reset( false );
      p() -> cooldown.sigil_of_silence -> reset( false );
    }
  }
};

// Nemesis ==================================================================

struct nemesis_t : public demon_hunter_spell_t
{
  nemesis_t( demon_hunter_t* p, const std::string& options_str ) : 
    demon_hunter_spell_t( "nemesis", p, p -> talent.nemesis )
  {
    parse_options( options_str );

    may_miss = may_block = may_dodge = may_parry = may_crit = false;
  }

  void impact( action_state_t* s ) override
  {
    demon_hunter_spell_t::impact( s );

    td( s -> target ) -> debuffs.nemesis -> trigger();
  }
};

// Sigil of Flame ===========================================================

struct sigil_of_flame_t : public demon_hunter_spell_t
{
  struct sigil_of_flame_damage_t : public demon_hunter_spell_t
  {
    sigil_of_flame_damage_t( demon_hunter_t* p ) :
      demon_hunter_spell_t( "sigil_of_flame_dmg", p, p -> find_spell( 204598 ) )
    {
      aoe = -1;
      background = dual = true;
      hasted_ticks = false;

      if ( p -> talent.concentrated_sigils -> ok() )
      {
        range = 0; // Targeted at player's location.
        dot_duration += p -> talent.concentrated_sigils -> effectN( 5 ).time_value();
      }
    }
  };

  sigil_of_flame_damage_t* damage;
  timespan_t delay;

  sigil_of_flame_t( demon_hunter_t* p, const std::string& options_str ) :
    demon_hunter_spell_t( "sigil_of_flame", p,
      p -> find_specialization_spell( "Sigil of Flame" ) )
  {
    parse_options( options_str );

    may_miss = may_crit = false;
    delay = data().duration() + p -> talent.quickened_sigils -> effectN( 1 ).time_value();
    damage = new sigil_of_flame_damage_t( p );
    damage -> stats = stats;

    // Add damage modifiers in sigil_of_flame_damage_t, not here.
  }

  /* Don't record data for this action, since we don't want that 0
     damage hit incorporated into statistics. */
  virtual void record_data( action_state_t* ) override {}

  void execute() override
  {
    demon_hunter_spell_t::execute();

    new ( *sim ) delayed_execute_event_t( p(), damage, target, delay );
  }
};

// Spirit Bomb ==============================================================

struct spirit_bomb_t : public demon_hunter_spell_t
{
  struct frail_t : public demon_hunter_spell_t
  {
    frail_t( demon_hunter_t* p ) :
      demon_hunter_spell_t( "frail", p, p -> find_spell( 218677 ) )
    {
      aoe = -1;
      background = dual = true;
    }

    void impact( action_state_t* s ) override
    {
      demon_hunter_spell_t::impact( s );

      if ( result_is_hit( s -> result ) )
      {
        td( s -> target ) -> debuffs.frail -> trigger();
      }
    }
  };

  frail_t* frail;

  spirit_bomb_t( demon_hunter_t* p, const std::string& options_str ) :
    demon_hunter_spell_t( "spirit_bomb", p, p -> talent.spirit_bomb )
  {
    parse_options( options_str );

    may_miss = may_crit = false;
    frail = new frail_t( p );
    frail -> stats = stats;
  }

  /* Don't record data for this action, since we don't want that 0
     damage hit incorporated into statistics. */
  virtual void record_data( action_state_t* ) override {}

  void consume_resource() override
  {
    demon_hunter_spell_t::consume_resource();

    if ( p() -> lesser_soul_fragments > 0 )
    {
      p() -> lesser_soul_fragments--;

      if ( sim -> debug )
      {
        sim -> out_debug.printf(
          "%s consumes 1 lesser soul fragment for %s. count=%u",
          p() -> name(), name(), p() -> lesser_soul_fragments );
      }
    }
    else
    {
      p() -> soul_fragments--;

      if ( sim -> debug )
      {
        sim -> out_debug.printf(
          "%s consumes 1 soul fragment for %s. count=%u",
          p() -> name(), name(), p() -> soul_fragments );
      }
    }
  }

  void execute() override
  {
    demon_hunter_spell_t::execute();

    frail -> target = target;
    frail -> schedule_execute();
  }

  bool ready() override
  {
    if ( p() -> lesser_soul_fragments + p() -> soul_fragments == 0 )
    {
      return false;
    }

    return demon_hunter_spell_t::ready();
  }
};

}  // end namespace spells

// ==========================================================================
// Demon Hunter attacks
// ==========================================================================

struct demon_hunter_attack_t : public demon_hunter_action_t<melee_attack_t>
{
  bool may_proc_fel_barrage;

  demon_hunter_attack_t( const std::string& n, demon_hunter_t* p,
                         const spell_data_t* s = spell_data_t::nil() )
    : base_t( n, p, s ), may_proc_fel_barrage( true )
  {
    special = true;

    parse_special_effect_data( data() );
  }

  void parse_special_effect_data( const spell_data_t& spell )
  {
    if ( spell.flags( SPELL_ATTR_EX3_REQ_OFFHAND ) )
    {
      weapon = &( p() -> off_hand_weapon );
    }
  }

  virtual void impact( action_state_t* s ) override
  {
    base_t::impact( s );

    if ( result_is_hit( s -> result ) )
    {
      trigger_fel_barrage( s );
    }
  }

  void trigger_fel_barrage( action_state_t* s )
  {
    if ( ! p() -> talent.fel_barrage -> ok() )
      return;

    if ( ! may_proc_fel_barrage )
      return;

    if ( p() -> cooldown.fel_barrage_proc -> down() )
      return;

    if ( s -> result_amount <= 0 )
      return;

    if ( ! p() -> rng().roll( p() -> spec.fel_barrage_proc -> proc_chance() ) )
      return;

    p() -> proc.fel_barrage -> occur();
    p() -> cooldown.fel_barrage -> reset( true );
    p() -> cooldown.fel_barrage_proc -> start();
  }
};

namespace attacks
{
// Melee Attack =============================================================

struct melee_t : public demon_hunter_attack_t
{
  enum status_e
  {
    MELEE_CONTACT,
    LOST_CONTACT_CHANNEL,
    LOST_CONTACT_RANGE,
  };

  struct
  {
    status_e main_hand, off_hand;
  } status;

  melee_t( const std::string& name, demon_hunter_t* p )
    : demon_hunter_attack_t( name, p, spell_data_t::nil() )
  {
    school     = SCHOOL_PHYSICAL;
    special    = false;
    background = repeating = auto_attack = may_glance = true;
    may_proc_fel_barrage = false; // Apr 22 2016
    trigger_gcd = timespan_t::zero();

    status.main_hand = status.off_hand = LOST_CONTACT_RANGE;

    if ( p -> dual_wield() )
    {
      base_hit -= 0.19;
    }
  }

  void reset() override
  {
    demon_hunter_attack_t::reset();

    status.main_hand = status.off_hand = LOST_CONTACT_RANGE;
  }

  timespan_t execute_time() const override
  {
    status_e s =
      weapon -> slot == SLOT_MAIN_HAND ? status.main_hand : status.off_hand;

    switch ( s )
    {
      /* Autoattacks after a channel occur around 350ms later. channel_lag
         takes care of the first 250, but add an extra 100 here. */
      case LOST_CONTACT_CHANNEL:
        return timespan_t::from_millis( 100 );
      // Position lag stuff
      case LOST_CONTACT_RANGE:
        return rng().gauss( timespan_t::from_millis( 250 ),
                            timespan_t::from_millis( 62 ) );
      default:
        return demon_hunter_attack_t::execute_time();
    }
  }

  void impact( action_state_t* s ) override
  {
    demon_hunter_attack_t::impact( s );

    if ( result_is_hit( s -> result ) )
    {
      trigger_demon_blades( s );
    }
  }

  void schedule_execute( action_state_t* s ) override
  {
    demon_hunter_attack_t::schedule_execute( s );

    if ( weapon -> slot == SLOT_MAIN_HAND )
      status.main_hand = MELEE_CONTACT;
    else if ( weapon -> slot == SLOT_OFF_HAND )
      status.off_hand = MELEE_CONTACT;
  }

  void execute() override
  {
    if ( p() -> current.distance_to_move > 5 || p() -> channeling ||
         ( p() -> buffs.self_movement -> check() &&
           !p() -> buff.vengeful_retreat_jump_cancel -> check() ) )
    {
      status_e s;

      // Cancel autoattacks, auto_attack_t will restart them when we're able to
      // attack again.
      if ( p() -> channeling )
      {
        p() -> proc.delayed_aa_channel -> occur();
        s = LOST_CONTACT_CHANNEL;
      }
      else
      {
        p() -> proc.delayed_aa_range -> occur();
        s = LOST_CONTACT_RANGE;
      }

      if ( weapon -> slot == SLOT_MAIN_HAND )
      {
        status.main_hand = s;
        player -> main_hand_attack -> cancel();
      }
      else
      {
        status.off_hand = s;
        player -> off_hand_attack -> cancel();
      }
      return;
    }

    demon_hunter_attack_t::execute();
  }

  void trigger_demon_blades( action_state_t* s )
  {
    if ( ! p() -> talent.demon_blades -> ok() )
      return;

    if ( p() -> in_gcd() )
      return;

    if ( p() -> active.demon_blades -> cooldown -> down() )
      return;

    if ( ! p() -> rppm.demon_blades -> trigger() )
      return;

    p() -> active.demon_blades -> target = s -> target;
    p() -> active.demon_blades -> schedule_execute();
  }
};

// Auto Attack ==============================================================

struct auto_attack_t : public demon_hunter_attack_t
{
  auto_attack_t( demon_hunter_t* p, const std::string& options_str )
    : demon_hunter_attack_t( "auto_attack", p )
  {
    parse_options( options_str );

    ignore_false_positive = true;
    range                 = 5;
    trigger_gcd           = timespan_t::zero();

    p -> melee_main_hand                     = new melee_t( "auto_attack_mh", p );
    p -> main_hand_attack                    = p -> melee_main_hand;
    p -> main_hand_attack -> weapon            = &( p -> main_hand_weapon );
    p -> main_hand_attack -> base_execute_time = p -> main_hand_weapon.swing_time;

    p -> melee_off_hand                     = new melee_t( "auto_attack_oh", p );
    p -> off_hand_attack                    = p -> melee_off_hand;
    p -> off_hand_attack -> weapon            = &( p -> off_hand_weapon );
    p -> off_hand_attack -> base_execute_time = p -> off_hand_weapon.swing_time;
    p -> off_hand_attack -> id                = 1;
  }

  void execute() override
  {
    if ( p() -> main_hand_attack -> execute_event == nullptr )
    {
      p() -> main_hand_attack -> schedule_execute();
    }

    if ( p() -> off_hand_attack -> execute_event == nullptr )
    {
      p() -> off_hand_attack -> schedule_execute();
    }
  }

  bool ready() override
  {
    bool ready = demon_hunter_attack_t::ready();

    if ( ready )  // Range check
    {
      if ( p() -> main_hand_attack -> execute_event == nullptr )
        return ready;

      if ( p() -> off_hand_attack -> execute_event == nullptr )
        return ready;
    }

    return false;
  }
};

// Blade Dance =============================================================

struct blade_dance_attack_t : public demon_hunter_attack_t
{
  blade_dance_attack_t( demon_hunter_t* p, const spell_data_t* s,
                        const std::string& name )
    : demon_hunter_attack_t( name, p, s )
  {
    dual = background = true;
    aoe = -1;
  }

  double action_multiplier() const override
  {
    double am = demon_hunter_attack_t::action_multiplier();

    am *= 1.0 + target_list().size() * p() -> artifact.balanced_blades.percent();

    return am;
  }

  double composite_da_multiplier( const action_state_t* s ) const override
  {
    double dm = demon_hunter_attack_t::composite_da_multiplier( s );

    if ( p() -> talent.first_blood -> ok() && s -> chain_target == 0 )
    {
      dm *= 1.0 + p() -> talent.first_blood -> effectN( 1 ).percent();
    }

    return dm;
  }
};

struct blade_dance_event_t : public event_t
{
  demon_hunter_t* dh;
  size_t current;
  bool in_metamorphosis;
  const spell_data_t* timing_passive;
  std::vector<attack_t*>* attacks;

  blade_dance_event_t( demon_hunter_t* p, size_t ca, bool meta = false )
    : event_t( *p -> sim ), dh( p ), current( ca ), in_metamorphosis( meta )
  {
    timing_passive =
      in_metamorphosis ? p -> spec.death_sweep : p -> spec.blade_dance;
    attacks =
      in_metamorphosis ? &p -> death_sweep_attacks : &p -> blade_dance_attacks;

    add_event( next_execute() );
  }

  const char* name() const override
  {
    return "Blade Dance";
  }

  timespan_t next_execute() const
  {
    if ( current == 0 )
    {
      return timespan_t::zero();
    }
    else
    {
      return timespan_t::from_millis(
        timing_passive -> effectN( current + 4 ).misc_value1()
        - timing_passive -> effectN( current + 3 ).misc_value1()
      );
    }
  }

  void execute() override
  {
    attacks -> at( current ) -> schedule_execute();
    current++;

    if ( current < attacks -> size() )
    {
      dh -> blade_dance_driver =
        new ( sim() ) blade_dance_event_t( dh, current, in_metamorphosis );
    }
    else
    {
      dh -> blade_dance_driver = nullptr;
    }
  }
};

struct blade_dance_base_t : public demon_hunter_attack_t
{
  std::vector<attack_t*> attacks;
  buff_t* dodge_buff;

  blade_dance_base_t( const std::string& n, demon_hunter_t* p,
                      const spell_data_t* s )
    : demon_hunter_attack_t( n, p, s ), dodge_buff( nullptr )
  {
    may_miss = may_crit = may_parry = may_block = may_dodge = false;
    cooldown = p -> get_cooldown( "blade_dance" );  // shared cooldown

    base_costs[ RESOURCE_FURY ] +=
      p -> talent.first_blood -> effectN( 2 ).resource( RESOURCE_FURY );
  }

  virtual bool init_finished() override
  {
    bool f = demon_hunter_attack_t::init_finished();

    for ( size_t i = 0; i < attacks.size(); i++ )
    {
      attacks[ i ] -> stats = stats;
    }

    return f;
  }

  /* Don't record data for this action, since we don't want that 0
     damage hit incorporated into statistics. */
  virtual void record_data( action_state_t* ) override {}

  virtual double action_multiplier() const override
  {
    double am = demon_hunter_attack_t::action_multiplier();

    am *= 1.0 + target_list().size() * p() -> artifact.balanced_blades.percent();

    return am;
  }

  virtual double composite_da_multiplier( const action_state_t* s ) const override
  {
    double dm = demon_hunter_attack_t::composite_da_multiplier( s );

    if ( p() -> talent.first_blood -> ok() && s -> chain_target == 0 )
    {
      dm *= 1.0 + p() -> talent.first_blood -> effectN( 1 ).percent();
    }

    return dm;
  }

  virtual void execute() override
  {
    cooldown -> duration = data().cooldown();

    demon_hunter_attack_t::execute();

    assert( !p() -> blade_dance_driver );
    p() -> blade_dance_driver = new ( *sim )
      blade_dance_event_t( p(), 0, p() -> buff.metamorphosis -> up() );

    assert( dodge_buff );
    dodge_buff -> trigger();
  }
};

struct blade_dance_t : public blade_dance_base_t
{
  blade_dance_t( demon_hunter_t* p, const std::string& options_str )
    : blade_dance_base_t( "blade_dance", p, p -> spec.blade_dance )
  {
    parse_options( options_str );

    attacks    = p -> blade_dance_attacks;
    dodge_buff = p -> buff.blade_dance;
  }

  bool ready() override
  {
    if ( p() -> buff.metamorphosis -> check() )
    {
      return false;
    }

    return blade_dance_base_t::ready();
  }
};

// Chaos Blades ============================================================

struct chaos_blade_t : public demon_hunter_attack_t
{
  chaos_blade_t( const std::string& n, demon_hunter_t* p, const spell_data_t* s )
    : demon_hunter_attack_t( n, p, s )
  {
    base_execute_time = weapon -> swing_time;
    special = true; // Apr 12 2016: Cannot miss.
    repeating = background = true;
  }
};

// Chaos Strike =============================================================

struct chaos_strike_state_t : public action_state_t
{
  bool is_critical;
  player_t* dh;

  chaos_strike_state_t( action_t* a, player_t* target ) :
    action_state_t( a, target ), dh( a -> player )
  {}

  void copy_state( const action_state_t* s ) override
  {
    action_state_t::copy_state( s );

    is_critical = debug_cast<const chaos_strike_state_t*>( s ) -> is_critical;
  }

  std::ostringstream& debug_str( std::ostringstream& s ) override
  {
    action_state_t::debug_str( s );

    s << " is_critical=" << is_critical;

    return s;
  }
};

struct chaos_strike_event_t : public event_t
{
  action_t* action;
  player_t* target;
  chaos_strike_state_t* state;

  chaos_strike_event_t( demon_hunter_t* p, action_t* a, player_t* t,
                        timespan_t delay, chaos_strike_state_t* s )
    : event_t( *p -> sim ), action( a ), target( t ), state( s )
  {
    add_event( delay );

    assert( action -> background );
  }

  const char* name() const override
  { return "chaos_strike_event"; }

  void execute() override
  {
    if ( ! target -> is_sleeping() )
    {
      chaos_strike_state_t* s = debug_cast<chaos_strike_state_t*>( action -> get_state() );
      s -> target = target;
      action -> snapshot_state( s, DMG_DIRECT );
      s -> is_critical = state -> is_critical;
      action -> schedule_execute( s );
    }
  }
};

struct chaos_strike_base_t : public demon_hunter_attack_t
{
  struct chaos_strike_damage_t : public demon_hunter_attack_t
  {
    chaos_strike_base_t* parent;
    bool may_refund;

    chaos_strike_damage_t( demon_hunter_t* p, const spell_data_t* s,
        chaos_strike_base_t* a ) :
      demon_hunter_attack_t( "chaos_strike_dmg", p, s ), parent( a )
    {
      dual = background = true;
      aoe = s -> effectN( 1 ).chain_target()
        + p -> talent.chaos_cleave -> effectN( 1 ).base_value();
      may_refund = weapon == &( p -> off_hand_weapon );

      base_multiplier *= 1.0 + p -> artifact.warglaives_of_chaos.percent();
      crit_bonus_multiplier *=  1.0 + p -> artifact.critical_chaos.percent();
    }

    action_state_t* new_state() override
    { return new chaos_strike_state_t( this, target ); }

    result_e calculate_result( action_state_t* s ) const override
    {
      result_e r = demon_hunter_attack_t::calculate_result( s );

      if ( result_is_miss( r ) )
      {
        return r;
      }
      
      /* Use the crit roll from our custom state instead of the roll done in
         attack_t::calculate_result. */
      return debug_cast<chaos_strike_state_t*>( s ) -> is_critical ?
        RESULT_CRIT : RESULT_HIT;
    }

    void schedule_execute( action_state_t* s = nullptr ) override
    {
      demon_hunter_attack_t::schedule_execute( s );

      /* Refund occurs prior to the offhand hit, and since we need to check the execute state
         we'll replicate that by doing it here instead of in execute(). */
      if ( may_refund && debug_cast<chaos_strike_state_t*>( s ) -> is_critical )
      {
        p() -> resource_gain( RESOURCE_FURY, parent -> cost(), parent -> action_gain );
      }
    }
  };

  std::vector<chaos_strike_damage_t*> attacks;
  std::vector<timespan_t> delays;

  chaos_strike_base_t( const std::string& n, demon_hunter_t* p,
                       const spell_data_t* s )
    : demon_hunter_attack_t( n, p, s )
  {
    aoe = s -> effectN( 1 ).chain_target()
      + p -> talent.chaos_cleave -> effectN( 1 ).base_value();

    // Create a child action for each "Triggers Spell" effect with the designated delay.
    for ( size_t i = 1; i <= s -> effect_count(); i++ ) {
      const spelleffect_data_t effect = s -> effectN( i );

      if ( effect.type() == E_TRIGGER_SPELL ) {
        attacks.push_back( new chaos_strike_damage_t( p, s -> effectN( i ).trigger(), this ) );
        delays.push_back( timespan_t::from_millis( s -> effectN( i ).misc_value1() ) );
      }
    }

    base_multiplier *= 1.0 + p -> artifact.warglaives_of_chaos.percent();
    crit_bonus_multiplier *= 1.0 + p -> artifact.critical_chaos.percent();
  }

  virtual bool init_finished() override
  {
    bool f = demon_hunter_attack_t::init_finished();

    // Use one stats object for all parts of the attack.
    for ( size_t i = 0; i < attacks.size(); i++ )
    {
      attacks[ i ] -> stats = stats;
    }

    return f;
  }

  virtual void record_data( action_state_t* s ) override
  {
    // Only record data if this action actually deals damage.
    if ( weapon && weapon_multiplier > 0 )
    {
      demon_hunter_attack_t::record_data( s );
    }
  }

  action_state_t* new_state() override
  { return new chaos_strike_state_t( this, target ); }

  void snapshot_state( action_state_t* s, dmg_e rt ) override
  {
    demon_hunter_attack_t::snapshot_state( s, rt );

    chaos_strike_state_t* cs = debug_cast<chaos_strike_state_t*>( s );

    /* If this is the primary target then roll for crit, otherwise copy the
       result from the previous chain_target's state. */
    if ( cs -> chain_target == 0 )
    {
      cs -> is_critical = p() -> rng().roll( cs -> composite_crit() );
    }
    else
    {
      assert( execute_state );
      cs -> is_critical = debug_cast<chaos_strike_state_t*>( execute_state ) -> is_critical;
    }
  }

  result_e calculate_result( action_state_t* s ) const override
  {
    result_e r = demon_hunter_attack_t::calculate_result( s );

    if ( result_is_miss( r ) )
    {
      return r;
    }

    /* Use the crit roll from our custom state instead of the roll done in
       attack_t::calculate_result. */
    return debug_cast<chaos_strike_state_t*>( s ) -> is_critical ?
      RESULT_CRIT : RESULT_HIT;
  }

  virtual void execute() override
  {
    demon_hunter_attack_t::execute();
    
    /* Trigger child actions. For Annihilation this is the MH and OH damage,
    and for Chaos Strike it's just the OH (this action is the MH action). */
    for ( size_t i = 0; i < attacks.size(); i++ )
    {
      new ( *sim ) chaos_strike_event_t( p(), attacks[ i ], target, delays[ i ],
        debug_cast<chaos_strike_state_t*>( execute_state ) );
    }

    // Metamorphosis benefit
    p() -> buff.metamorphosis -> up();

    if ( hit_any_target )
    {
      // TODO: Travel time
      if ( p() -> talent.demonic_appetite -> ok() &&
           ! p() -> cooldown.demonic_appetite -> down() &&
           p() -> rng().roll( p() -> talent.demonic_appetite -> proc_chance() ) )
      {
        p() -> cooldown.demonic_appetite -> start();
        p() -> proc.demonic_appetite -> occur();

        p() -> spawn_soul_fragment_lesser();
        p() -> consume_soul_fragments();
      }

      // Inner Demons procs on cast
      if ( p() -> rppm.inner_demons -> trigger() )
      {
        assert( p() -> active.inner_demons );
        p() -> active.inner_demons -> target = target;
        p() -> active.inner_demons -> schedule_execute();
      }
    }
  }
};

struct chaos_strike_t : public chaos_strike_base_t
{
  chaos_strike_t( demon_hunter_t* p, const std::string& options_str )
    : chaos_strike_base_t( "chaos_strike", p, p -> spec.chaos_strike )
  {
    parse_options( options_str );

    aoe += p -> talent.chaos_cleave -> effectN( 1 ).base_value();
  }

  bool ready() override
  {
    if ( p() -> buff.metamorphosis -> check() )
    {
      return false;
    }

    return chaos_strike_base_t::ready();
  }
};

// Annihilation =============================================================

struct annihilation_t : public chaos_strike_base_t
{
  annihilation_t( demon_hunter_t* p, const std::string& options_str )
    : chaos_strike_base_t( "annihilation", p, p -> spec.annihilation )
  {
    parse_options( options_str );
  }

  bool ready() override
  {
    if ( !p() -> buff.metamorphosis -> check() )
    {
      return false;
    }

    return chaos_strike_base_t::ready();
  }
};

// Death Sweep ==============================================================

struct death_sweep_t : public blade_dance_base_t
{
  death_sweep_t( demon_hunter_t* p, const std::string& options_str )
    : blade_dance_base_t( "death_sweep", p, p -> spec.death_sweep )
  {
    parse_options( options_str );

    attacks    = p -> death_sweep_attacks;
    dodge_buff = p -> buff.death_sweep;
  }

  bool ready() override
  {
    if ( !p() -> buff.metamorphosis -> check() )
    {
      return false;
    }

    return blade_dance_base_t::ready();
  }
};

// Demon's Bite =============================================================

struct demons_bite_t : public demon_hunter_attack_t
{
  struct
  {
    int base, die_sides;
  } fury;

  demons_bite_t( demon_hunter_t* p, const std::string& options_str )
    : demon_hunter_attack_t( "demons_bite", p,
                             p -> find_class_spell( "Demon's Bite" ) )
  {
    parse_options( options_str );

    fury.base      = data().effectN( 3 ).resource( RESOURCE_FURY );
    fury.die_sides = data().effectN( 3 ).die_sides();
    energize_type  = ENERGIZE_NONE; // disable, we need to do it manually for the RNG

    base_multiplier *= 1.0 + p -> artifact.demon_rage.percent();

    if ( p -> legendary.anger_of_the_halfgiants )
    {
      fury.die_sides += p -> legendary.anger_of_the_halfgiants -> driver() -> effectN( 1 ).resource( RESOURCE_FURY );
    }
  }

  void execute() override
  {
    demon_hunter_attack_t::execute();

    if ( p() -> buff.metamorphosis -> check() )
    {
      p() -> proc.demons_bite_in_meta -> occur();
    }
  }

  void impact( action_state_t* s ) override
  {
    demon_hunter_attack_t::impact( s );

    if ( result_is_hit( s -> result ) )
    {
      trigger_fury_gain();
      
      if ( p() -> talent.felblade -> ok() && p() -> rppm.felblade -> trigger() )
      {
        p() -> proc.felblade_reset -> occur();
        p() -> cooldown.felblade -> reset( true );
      }
    }

  }

  bool ready() override
  {
    if ( p() -> talent.demon_blades -> ok() )
    {
      return false;
    }

    return demon_hunter_attack_t::ready();
  }

  void trigger_fury_gain()
  {
    double f = fury.base + rng().range( 1, 1 + fury.die_sides );
    // Generates an integer amount of fury.
    p() -> resource_gain( RESOURCE_FURY, ( int ) f, action_gain );
  }
};

// Demon Blade ==============================================================

struct demon_blades_t : public demon_hunter_attack_t
{
  struct
  {
    double base, die_sides;
  } fury;

  demon_blades_t( demon_hunter_t* p )
    : demon_hunter_attack_t( "demon_blades", p, p -> find_spell( 203796 ) )
  {
    background           = true;
    may_proc_fel_barrage = false; // Apr 22 2016
    cooldown -> duration = p -> talent.demon_blades -> internal_cooldown();
    fury.base            = data().effectN( 3 ).resource( RESOURCE_FURY );
    fury.die_sides       = data().effectN( 3 ).die_sides();

    if ( p -> legendary.anger_of_the_halfgiants )
    {
      fury.die_sides += p -> legendary.anger_of_the_halfgiants -> driver() -> effectN( 1 ).resource( RESOURCE_FURY );
    }
  }

  void impact( action_state_t* s ) override
  {
    demon_hunter_attack_t::impact( s );

    if ( result_is_hit( s -> result ) )
    {
      trigger_fury_gain();
    }

    if ( p() -> talent.felblade -> ok() && p() -> rppm.felblade -> trigger() )
    {
      p() -> proc.felblade_reset -> occur();
      p() -> cooldown.felblade -> reset( true );
    }
  }

  void trigger_fury_gain()
  {
    double f = fury.base + rng().range( 1, 1 + fury.die_sides );
    // Generates an integer amount of fury.
    p() -> resource_gain( RESOURCE_FURY, ( int ) f, action_gain );
  }
};

// Felblade =================================================================
// TODO: Real movement stuff.

struct felblade_t : public demon_hunter_attack_t
{
  struct felblade_damage_t : public demon_hunter_attack_t
  {
    felblade_damage_t( demon_hunter_t* p ) :
      demon_hunter_attack_t( "felblade_dmg", p, p -> find_spell( 213243 ) )
    {
      dual = background = true;
      may_miss = may_dodge = may_parry = false;

      // Clear energize and then manually pick which effect to parse.
      energize_type = ENERGIZE_NONE;
      parse_effect_data( data().effectN( p -> specialization() == DEMON_HUNTER_HAVOC ? 4 : 3 ) );
    }
  };

  felblade_t( demon_hunter_t* p, const std::string& options_str ) :
    demon_hunter_attack_t( "felblade", p, p -> talent.felblade )
  {
    parse_options( options_str );

    may_crit = may_block = false;
    impact_action = new felblade_damage_t( p );
    impact_action -> stats = stats;

    movement_directionality = MOVEMENT_TOWARDS;

    // Add damage modifiers in felblade_damage_t, not here.
  }

  /* Don't record data for this action, since we don't want that 0
     damage hit incorporated into statistics. */
  void record_data( action_state_t* ) override {}
};

// Fracture =================================================================

struct fracture_t : public demon_hunter_attack_t
{
  fracture_t( demon_hunter_t* p, const std::string& options_str ) :
    demon_hunter_attack_t( "fracture", p, p -> talent.fracture )
  {
    parse_options( options_str );
  }

  void execute() override
  {
    demon_hunter_attack_t::execute();

    p() -> spawn_soul_fragment_lesser( 2 );
  }
};

// Fury of the Illidari =====================================================
// TODO: Distance targeting support.

struct fury_of_the_illidari_t : public demon_hunter_attack_t
{
  struct rage_of_the_illidari_t : public demon_hunter_attack_t
  {
    rage_of_the_illidari_t( demon_hunter_t* p )
      : demon_hunter_attack_t( "rage_of_the_illidari", p,
                               p -> find_spell( 217070 ) )
    {
      may_crit   = false;
      aoe        = -1;
      background = split_aoe_damage = true;
      base_multiplier = 1.0;  // 100% of damage dealt; not in spell data
    }

    void init() override
    {
      demon_hunter_attack_t::init();

      snapshot_flags = update_flags = 0;
    }

    void execute() override
    {
      // Manually apply base_multiplier since the flag it uses is disabled.
      base_dd_min = base_dd_max =
        base_multiplier * p() -> buff.rage_of_the_illidari -> check_value();

      demon_hunter_attack_t::execute();

      p() -> buff.rage_of_the_illidari -> expire();
    }
  };

  struct fury_of_the_illidari_tick_t : public demon_hunter_attack_t
  {
    fury_of_the_illidari_tick_t( demon_hunter_t* p, const spell_data_t* s )
      : demon_hunter_attack_t( "fury_of_the_illidari_tick", p, s )
    {
      background = dual = ground_aoe = true;
      aoe = -1;
    }

    void impact( action_state_t* s ) override
    {
      demon_hunter_attack_t::impact( s );

      if ( result_is_hit( s -> result ) && p() -> artifact.rage_of_the_illidari.rank() )
      {
        p() -> buff.rage_of_the_illidari -> trigger( 1,
          p() -> buff.rage_of_the_illidari -> current_value + s -> result_amount );
      }
    }

    dmg_e amount_type( const action_state_t*, bool ) const override
    {
      return DMG_OVER_TIME;
    }  // TOCHECK
  };

  fury_of_the_illidari_tick_t* mh;
  fury_of_the_illidari_tick_t* oh;
  rage_of_the_illidari_t* rage;

  fury_of_the_illidari_t( demon_hunter_t* p, const std::string& options_str )
    : demon_hunter_attack_t( "fury_of_the_illidari", p,
                             p -> artifact.fury_of_the_illidari ),
      mh( new fury_of_the_illidari_tick_t( p, p -> find_spell( 201628 ) ) ),
      oh( new fury_of_the_illidari_tick_t( p, p -> find_spell( 201789 ) ) ),
      rage( nullptr )
  {
    parse_options( options_str );

    may_miss = may_crit = may_parry = may_block = may_dodge = false;
    dot_duration   = data().duration();
    base_tick_time = timespan_t::from_millis( 500 );
    tick_zero = true;

    // Set MH to tick action. OH is executed in tick().
    tick_action = mh;

    // Silly reporting things
    school    = mh -> school;
    mh -> stats = oh -> stats = stats;

    if ( p -> artifact.rage_of_the_illidari.rank() )
    {
      rage = new rage_of_the_illidari_t( p );
      add_child( rage );
    }
  }

  timespan_t travel_time() const override
  {
    return timespan_t::zero();
  }

  void tick( dot_t* d ) override
  {
    demon_hunter_attack_t::tick( d );

    oh -> schedule_execute();
  }

  void last_tick( dot_t* d ) override
  {
    demon_hunter_attack_t::last_tick( d );

    if ( p() -> artifact.rage_of_the_illidari.rank() )
    {
      rage -> schedule_execute();
    }
  }
};

// Shear ====================================================================

struct shear_t : public demon_hunter_attack_t
{
  std::array<double, 8> shatter_chance;

  shear_t( demon_hunter_t* p, const std::string& options_str ) :
    demon_hunter_attack_t( "shear", p, p -> find_specialization_spell( "Shear" ) )
  {
    parse_options( options_str );

    /* Proc chance increases on each consecutive failed attempt, rates
       from http://us.battle.net/wow/en/forum/topic/20743504316?page=4#75 */
    shatter_chance = { 0.04, 0.12, 0.25, 0.40, 0.60, 0.80, 0.90, 1.00 };

    base_multiplier *= 1.0 + p -> artifact.honed_warblades.percent();
  }

  void shatter( action_state_t* s )
  {
    assert( p() -> shear_counter < 8 );

    double chance = shatter_chance[ p() -> shear_counter ]
      + td( s -> target ) -> debuffs.frail -> value();

    if ( p() -> health_percentage() < 50.0 )
    {
      chance += p() -> artifact.shatter_the_souls.percent();
    }

    if ( p() -> rng().roll( chance ) )
    {
      p() -> spawn_soul_fragment_lesser();
      p() -> shear_counter = 0;
    }
    else
    {
      p() -> shear_counter++;
    }
  }

  void impact( action_state_t* s ) override
  {
    demon_hunter_attack_t::impact( s );

    if ( result_is_hit( s -> result ) )
    {
      if ( p() -> talent.felblade -> ok() && p() -> rppm.felblade -> trigger() )
      {
        p() -> proc.felblade_reset -> occur();
        p() -> cooldown.felblade -> reset( true );
      }
    }
  }

  void execute() override
  {
    demon_hunter_attack_t::execute();

    if ( hit_any_target )
    {
      p() -> resource_gain( RESOURCE_PAIN, data().effectN( 3 ).resource( RESOURCE_PAIN ), action_gain );

      shatter( execute_state );
    }
  }
};

// Soul Carver ==============================================================

struct soul_carver_t : public demon_hunter_attack_t
{
  struct soul_carver_oh_t : public demon_hunter_attack_t
  {
    soul_carver_oh_t( demon_hunter_t* p ) :
      demon_hunter_attack_t( "soul_carver_oh", p,
        p -> artifact.soul_carver.data().effectN( 4 ).trigger() )
    {
      background = dual = true;
      may_miss = may_parry = may_dodge = false; // TOCHECK
    }
  };

  soul_carver_oh_t* oh;

  soul_carver_t( demon_hunter_t* p, const std::string& options_str ) :
    demon_hunter_attack_t( "soul_carver", p, p -> artifact.soul_carver )
  {
    parse_options( options_str );

    impact_action = new soul_carver_oh_t( p );
    impact_action -> stats = stats;
  }

  void impact( action_state_t* s ) override
  {
    demon_hunter_attack_t::impact( s );

    if ( result_is_hit( s -> result ) )
    {
      p() -> spawn_soul_fragment_lesser( 2 );
    }
  }

  void tick( dot_t* d ) override
  {
    demon_hunter_attack_t::tick( d );

    p() -> spawn_soul_fragment_lesser();
  }
};

// Soul Cleave ==============================================================

struct soul_cleave_t : public demon_hunter_attack_t
{
  struct soul_cleave_damage_t : public demon_hunter_attack_t
  {
    soul_cleave_damage_t( demon_hunter_t* p, const std::string& n, spell_data_t* s ) :
      demon_hunter_attack_t( n, p, s )
    {
      aoe = 1;
      background = dual = true;

      base_multiplier *= 1.0 + p -> artifact.tormented_souls.percent();
    }
  };

  heals::soul_cleave_heal_t* heal;
  soul_cleave_damage_t* mh, *oh;

  soul_cleave_t( demon_hunter_t* p, const std::string& options_str ) :
    demon_hunter_attack_t( "soul_cleave", p, p -> spec.soul_cleave ),
    heal( new heals::soul_cleave_heal_t( p ) )
  {
    parse_options( options_str );

    may_miss = may_dodge = may_parry = may_block = may_crit = false;
    attack_power_mod.direct = 0; // This parent action deals no damage;

    mh = new soul_cleave_damage_t( p, "soul_cleave_mh", data().effectN( 2 ).trigger() );
    oh = new soul_cleave_damage_t( p, "soul_cleave_oh", data().effectN( 3 ).trigger() );

    mh -> stats = stats;
    oh -> stats = stats;

    // Add damage modifiers in soul_cleave_damage_t, not here.
  }

  /* Don't record data for this action, since we don't want that 0
     damage hit incorporated into statistics. */
  virtual void record_data( action_state_t* ) override {}

  void execute() override
  {
    demon_hunter_attack_t::execute();
    
    // Heal happens first;
    heal -> schedule_execute();
    mh -> schedule_execute();
    oh -> schedule_execute();
    p() -> consume_soul_fragments();

    if ( p() -> legendary.the_defilers_lost_vambraces )
    {
      unsigned roll = ( unsigned ) p() -> rng().range( 0, p() -> sigil_cooldowns.size() );
      timespan_t t = timespan_t::from_seconds(
        p() -> legendary.the_defilers_lost_vambraces -> driver() -> effectN( 1 ).base_value() );
      p() -> sigil_cooldowns[ roll ] -> adjust( -t );
    }
  }
};

// Throw Glaive =============================================================

struct throw_glaive_t : public demon_hunter_attack_t
{
  struct bloodlet_t : public residual_action::residual_periodic_action_t <
    demon_hunter_attack_t >
  {
    bloodlet_t( demon_hunter_t* p )
      : residual_action::residual_periodic_action_t<demon_hunter_attack_t>(
          "bloodlet", p, p -> find_spell( 207690 ) )
    {
      background = dual = proc = true;
      may_miss = may_dodge = may_parry = false;
    }
  };

  bloodlet_t* bloodlet;

  throw_glaive_t( demon_hunter_t* p, const std::string& options_str )
    : demon_hunter_attack_t( "throw_glaive", p,
                             p -> find_class_spell( "Throw Glaive" ) ),
      bloodlet( nullptr )
  {
    parse_options( options_str );

    cooldown -> charges +=
      p -> talent.master_of_the_glaive -> effectN( 2 ).base_value();
    aoe                = 3;     // Ricochets to 2 additional enemies
    radius             = 10.0;  // with 10 yards.

    if ( p -> talent.bloodlet -> ok() )
    {
      bloodlet = new bloodlet_t( p );
      add_child( bloodlet );
    }

    base_multiplier *= 1.0 + p -> artifact.sharpened_glaives.percent();

    if ( p -> legendary.moarg_bionic_stabilizers )
    {
      base_add_multiplier *= 1.0 + p -> legendary.moarg_bionic_stabilizers
        -> driver() -> effectN( 1 ).percent();
    }
  }

  void impact( action_state_t* s ) override
  {
    demon_hunter_attack_t::impact( s );

    if ( p() -> talent.bloodlet -> ok() && result_is_hit( s -> result ) )
    {
      residual_action::trigger(
        bloodlet, s -> target,
        s -> result_amount * p() -> talent.bloodlet -> effectN( 1 ).percent() );
    }
  }
};

// Vengeful Retreat =========================================================

struct vengeful_retreat_t : public demon_hunter_attack_t
{
  struct vengeful_retreat_damage_t : public demon_hunter_attack_t
  {
    vengeful_retreat_damage_t( demon_hunter_t* p ) :
      demon_hunter_attack_t( "vengeful_retreat_dmg", p, p -> find_spell( 198813 ) )
    {
      background = dual = true;
      aoe = -1;
      may_proc_fel_barrage = false; // Apr 22 2016
    }
  };

  bool jump_cancel;

  vengeful_retreat_t( demon_hunter_t* p, const std::string& options_str )
    : demon_hunter_attack_t( "vengeful_retreat", p,
                             p -> find_class_spell( "Vengeful Retreat" ) ),
      jump_cancel( false )
  {
    add_option( opt_bool( "jump_cancel", jump_cancel ) );
    parse_options( options_str );

    may_miss = may_dodge = may_parry = may_crit = may_block = false;
    impact_action = new vengeful_retreat_damage_t( p );
    impact_action -> stats = stats;
    if ( ! jump_cancel )
    {
      base_teleport_distance  = 20.0;
      movement_directionality = MOVEMENT_OMNI;
    }
    ignore_false_positive = true;
    use_off_gcd           = true;

    cooldown -> duration += p -> talent.prepared -> effectN( 2 ).time_value();
    
    // Add damage modifiers in vengeful_retreat_damage_t, not here.
  }

  /* Don't record data for this action, since we don't want that 0
     damage hit incorporated into statistics. */
  virtual void record_data( action_state_t* ) override {}

  void execute() override
  {
    demon_hunter_attack_t::execute();

    if ( hit_any_target )
    {
      p() -> buff.prepared -> trigger();
    }

    p() -> buff.momentum -> trigger();

    // Buff to track the movement. This lets us delay autoattacks and other
    // things.
    if ( jump_cancel )
    {
      p() -> buff.vengeful_retreat_jump_cancel -> trigger();
      p() -> buffs.self_movement -> trigger(
        1, 0, -1.0, p() -> buff.vengeful_retreat_jump_cancel -> buff_duration );
    }
    else
    {
      p() -> buffs.self_movement -> trigger( 1, 0, -1.0,
                                         timespan_t::from_seconds( 1.0 ) );
    }
  }

  bool ready() override
  {
    if ( p() -> buffs.self_movement -> check() )
      return false;
    // Don't let the player try to jump cancel while out of range.
    if ( jump_cancel && p() -> current.distance_to_move > 5 )
      return false;

    return demon_hunter_attack_t::ready();
  }
};

}  // end namespace attacks

}  // end namespace actions

namespace buffs
{
template <typename BuffBase>
struct demon_hunter_buff_t : public BuffBase
{
protected:
  typedef demon_hunter_buff_t base_t;
  demon_hunter_t& dh;

public:
  demon_hunter_buff_t( demon_hunter_t& p, const buff_creator_basics_t& params )
    : BuffBase( params ), dh( p )
  {
  }

  demon_hunter_t& p() const
  {
    return dh;
  }
};

// Anguish ==================================================================

struct anguish_debuff_t : public demon_hunter_buff_t<buff_t>
{
  action_t* anguish;

  anguish_debuff_t( demon_hunter_t* p, player_t* target )
    : demon_hunter_buff_t<buff_t>(
        *p, buff_creator_t( target, "anguish",
                            p -> artifact.anguish_of_the_deceiver.data()
                            .effectN( 1 )
                            .trigger() ) )
  {
    anguish = p -> find_action( "anguish" );
  }

  virtual void expire_override( int expiration_stacks,
                                timespan_t remaining_duration ) override
  {
    demon_hunter_buff_t<buff_t>::expire_override( expiration_stacks,
      remaining_duration );

    // Only if the debuff expires naturally; if the target dies it doesn't deal
    // damage.
    if ( remaining_duration == timespan_t::zero() )
    {
      // Schedule an execute, but snapshot state right now so we can apply the
      // stack multiplier.
      action_state_t* s = anguish -> get_state();
      s -> target         = player;
      anguish -> snapshot_state( s, DMG_DIRECT );
      s -> target_da_multiplier *= expiration_stacks;
      anguish -> schedule_execute( s );
    }
  }
};

// Chaos Blades =============================================================

struct chaos_blades_t : public demon_hunter_buff_t<buff_t>
{
  chaos_blades_t( demon_hunter_t* p ) :
    demon_hunter_buff_t<buff_t>(
        *p, buff_creator_t( p, "chaos_blades", p -> talent.chaos_blades )
        .cd( timespan_t::zero() )
        .add_invalidate( CACHE_PLAYER_DAMAGE_MULTIPLIER ) )
  {}

  void change_auto_attack( attack_t*& hand, attack_t* a )
  {
    if ( hand == 0 )
      return;

    bool executing         = hand -> execute_event != 0;
    timespan_t time_to_hit = timespan_t::zero();

    if ( executing )
    {
      time_to_hit = hand -> execute_event -> occurs() - sim -> current_time();
      event_t::cancel( hand -> execute_event );
    }

    hand = a;

    // Kick off the new attack, by instantly scheduling and rescheduling it to
    // the remaining time to hit. We cannot use normal reschedule mechanism
    // here (i.e., simply use event_t::reschedule() and leave it be), because
    // the rescheduled event would be triggered before the full swing time
    // (of the new auto attack) in most cases.
    if ( executing )
    {
      timespan_t old_swing_time = hand -> base_execute_time;
      hand -> base_execute_time   = timespan_t::zero();
      hand -> schedule_execute();
      hand -> base_execute_time = old_swing_time;
      hand -> execute_event -> reschedule( time_to_hit );
    }
  }

  void execute( int stacks = 1, double value = buff_t::DEFAULT_VALUE(),
                timespan_t duration = timespan_t::min() )
  {
    buff_t::execute( stacks, value, duration );

    change_auto_attack( p().main_hand_attack, p().chaos_blade_main_hand );
    if ( p().off_hand_attack )
      change_auto_attack( p().off_hand_attack, p().chaos_blade_off_hand );
  }

  void expire_override( int expiration_stacks,
                        timespan_t remaining_duration ) override
  {
    buff_t::expire_override( expiration_stacks, remaining_duration );

    change_auto_attack( p().main_hand_attack, p().melee_main_hand );
    if ( p().off_hand_attack )
      change_auto_attack( p().off_hand_attack, p().melee_off_hand );
  }
};

// Nemesis ==================================================================

struct nemesis_debuff_t : public demon_hunter_buff_t<buff_t>
{
  nemesis_debuff_t( demon_hunter_t* p, player_t* target )
    : demon_hunter_buff_t<buff_t>(
        *p, buff_creator_t( target, "nemesis", p -> talent.nemesis )
        .default_value( p -> talent.nemesis -> effectN( 1 ).percent() )
        .cd( timespan_t::zero() ) )
  {
  }

  virtual void expire_override( int expiration_stacks,
                                timespan_t remaining_duration ) override
  {
    demon_hunter_buff_t<buff_t>::expire_override( expiration_stacks,
      remaining_duration );

    if ( remaining_duration > timespan_t::zero() )
    {
      p().buff.nemesis -> trigger( 1, player -> race, -1.0, remaining_duration );
    }
  }
};

// Metamorphosis Buff =======================================================

struct metamorphosis_buff_t : public demon_hunter_buff_t<buff_t>
{
  static void callback( buff_t* b, int, const timespan_t& )
  {
    demon_hunter_t* p = debug_cast<demon_hunter_t*>( b -> player );
    p -> resource_gain( RESOURCE_PAIN,
      p -> spec.metamorphosis_buff -> effectN( 4 ).resource( RESOURCE_PAIN ),
      p -> gain.metamorphosis );
  }

  metamorphosis_buff_t( demon_hunter_t* p ) : 
    demon_hunter_buff_t<buff_t>( *p,
      buff_creator_t( p, "metamorphosis", p -> spec.metamorphosis_buff )
        .cd( timespan_t::zero() )
        .default_value( p -> spec.metamorphosis_buff -> effectN( 2 ).percent()
          + p -> artifact.embrace_the_pain.percent() )
        .period( p -> spec.metamorphosis_buff -> effectN( 4 ).period() )
        .tick_callback( &callback )
        .add_invalidate( CACHE_LEECH ) )
  {}

  void start( int stacks, double value, timespan_t duration ) override
  {
    demon_hunter_buff_t<buff_t>::start( stacks, value, duration );

    p().metamorphosis_health = p().max_health() * value;
    p().stat_gain( STAT_MAX_HEALTH, p().metamorphosis_health,
      (gain_t*) nullptr, (action_t*) nullptr, true );
  }

  void expire_override( int expiration_stacks, timespan_t remaining_duration ) override
  {
    demon_hunter_buff_t<buff_t>::expire_override( expiration_stacks, remaining_duration );
    
    p().stat_loss( STAT_MAX_HEALTH, p().metamorphosis_health,
      (gain_t*) nullptr, (action_t*) nullptr, true );
    p().metamorphosis_health = 0;
  }
};

}  // end namespace buffs

// ==========================================================================
// Targetdata Definitions
// ==========================================================================

demon_hunter_td_t::demon_hunter_td_t( player_t* target, demon_hunter_t& p )
  : actor_target_data_t( target, &p ), dots( dots_t() ), debuffs( debuffs_t() )
{
  // Havoc
  debuffs.anguish     = new buffs::anguish_debuff_t( &p, target );
  debuffs.nemesis     = new buffs::nemesis_debuff_t( &p, target );

  // Vengeance
  dots.fiery_brand    = target -> get_dot( "fiery_brand_dot", &p );
  debuffs.frail       = buff_creator_t( target, "frail", p.find_spell( 218677 ) )
                          .default_value( 0.10 ); // 10% via http://us.battle.net/wow/en/forum/topic/20743504316?page=4#75
}

// ==========================================================================
// Demon Hunter Definitions
// ==========================================================================

demon_hunter_t::demon_hunter_t( sim_t* sim, const std::string& name, race_e r )
  : player_t( sim, DEMON_HUNTER, name, r ),
    blade_dance_driver( nullptr ),
    blade_dance_attacks( 0 ),
    death_sweep_attacks( 0 ),
    melee_main_hand( nullptr ),
    melee_off_hand( nullptr ),
    chaos_blade_main_hand( nullptr ),
    chaos_blade_off_hand( nullptr ),
    soul_fragments( 0 ),
    lesser_soul_fragments( 0 ),
    sigil_cooldowns( 0 ),
    buff(),
    talent(),
    spec(),
    mastery_spell(),
    cooldown(),
    gain(),
    benefits(),
    proc(),
    active(),
    pets(),
    options(),
    glyphs(),
    legendary()
{
  base.distance = 5.0;

  create_cooldowns();
  create_gains();
  create_benefits();

  regen_type = REGEN_DISABLED;
}

// ==========================================================================
// overridden player_t init functions
// ==========================================================================

// demon_hunter_t::convert_hybrid_stat ======================================

stat_e demon_hunter_t::convert_hybrid_stat( stat_e s ) const
{
  // this converts hybrid stats that either morph based on spec or only work
  // for certain specs into the appropriate "basic" stats
  switch ( s )
  {
    case STAT_STR_AGI_INT:
    case STAT_AGI_INT:
    case STAT_STR_AGI:
      return STAT_AGILITY;
    case STAT_STR_INT:
      return STAT_NONE;
    case STAT_SPIRIT:
      return STAT_NONE;
    case STAT_BONUS_ARMOR:
      return specialization() == DEMON_HUNTER_VENGEANCE ? s : STAT_NONE;
    default:
      return s;
  }
}

// demon_hunter_t::copy_from ================================================

void demon_hunter_t::copy_from( player_t* source )
{
  base_t::copy_from( source );

  auto source_p = debug_cast<demon_hunter_t*>( source );

  options = source_p -> options;
}

// demon_hunter_t::create_action ============================================

action_t* demon_hunter_t::create_action( const std::string& name,
  const std::string& options_str )
{
  using namespace actions::spells;

  if ( name == "blur" )
    return new blur_t( this, options_str );
  if ( name == "chaos_blades" )
    return new chaos_blades_t( this, options_str );
  if ( name == "chaos_nova" )
    return new chaos_nova_t( this, options_str );
  if ( name == "consume_magic" )
    return new consume_magic_t( this, options_str );
  if ( name == "demon_spikes" )
    return new demon_spikes_t( this, options_str );
  if ( name == "eye_beam" )
    return new eye_beam_t( this, options_str );
  if ( name == "empower_wards" )
    return new empower_wards_t( this, options_str );
  if ( name == "fel_barrage" )
    return new fel_barrage_t( this, options_str );
  if ( name == "fel_eruption" )
    return new fel_eruption_t( this, options_str );
  if ( name == "fel_devastation" )
    return new fel_devastation_t( this, options_str );
  if ( name == "fel_rush" )
    return new fel_rush_t( this, options_str );
  if ( name == "fiery_brand" )
    return new fiery_brand_t( this, options_str );
  if ( name == "immolation_aura" )
    return new immolation_aura_t( this, options_str );
  if ( name == "metamorphosis" || name == "meta" )
    return new metamorphosis_t( this, options_str );
  if ( name == "nemesis" )
    return new nemesis_t( this, options_str );
  if ( name == "sigil_of_flame" )
    return new sigil_of_flame_t( this, options_str );
  if ( name == "spirit_bomb" )
    return new spirit_bomb_t( this, options_str );

  using namespace actions::attacks;

  if ( name == "auto_attack" )
    return new auto_attack_t( this, options_str );
  if ( name == "annihilation" )
    return new annihilation_t( this, options_str );
  if ( name == "blade_dance" )
    return new blade_dance_t( this, options_str );
  if ( name == "chaos_strike" )
    return new chaos_strike_t( this, options_str );
  if ( name == "death_sweep" )
    return new death_sweep_t( this, options_str );
  if ( name == "demons_bite" )
    return new demons_bite_t( this, options_str );
  if ( name == "felblade" )
    return new felblade_t( this, options_str );
  if ( name == "fracture" )
    return new fracture_t( this, options_str );
  if ( name == "fury_of_the_illidari" )
    return new fury_of_the_illidari_t( this, options_str );
  if ( name == "shear" )
    return new shear_t( this, options_str );
  if ( name == "soul_carver" )
    return new soul_carver_t( this, options_str );
  if ( name == "soul_cleave" )
    return new soul_cleave_t( this, options_str );
  if ( name == "throw_glaive" )
    return new throw_glaive_t( this, options_str );
  if ( name == "vengeful_retreat" )
    return new vengeful_retreat_t( this, options_str );

  return base_t::create_action( name, options_str );
}

// demon_hunter_t::create_buffs =============================================

void demon_hunter_t::create_buffs()
{
  base_t::create_buffs();

  using namespace buffs;

  // General
  if ( specialization() == DEMON_HUNTER_HAVOC )
  {
    buff.metamorphosis =
      buff_creator_t( this, "metamorphosis", spec.metamorphosis_buff )
        .cd( timespan_t::zero() )
        .add_invalidate( CACHE_LEECH )
        .tick_behavior( BUFF_TICK_NONE );
  }
  else
  {
    buff.metamorphosis = new metamorphosis_buff_t( this );
  }
  
  // Havoc

  buff.blade_dance =
    buff_creator_t( this, "blade_dance", spec.blade_dance )
      .default_value( spec.blade_dance -> effectN( 2 ).percent() )
      .add_invalidate( CACHE_DODGE );

  buff.blur =
    buff_creator_t( this, "blur", spec.blur -> effectN( 1 ).trigger() )
      .default_value( spec.blur -> effectN( 1 ).trigger() -> effectN( 3 ).percent() )
      .cd( timespan_t::zero() )
      .add_invalidate( CACHE_LEECH );

  buff.chaos_blades = new chaos_blades_t( this );

  buff.death_sweep =
    buff_creator_t( this, "death_sweep", spec.death_sweep )
      .default_value( spec.death_sweep -> effectN( 2 ).percent() )
      .add_invalidate( CACHE_DODGE );

  buff.momentum =
    buff_creator_t( this, "momentum", find_spell( 208628 ) )
      .default_value( find_spell( 208628 ) -> effectN( 1 ).percent() )
      .trigger_spell( talent.momentum )
      .add_invalidate( CACHE_PLAYER_DAMAGE_MULTIPLIER );

  // TODO: Buffs for each race?
  buff.nemesis = buff_creator_t( this, "nemesis", find_spell( 208605 ) )
                   .add_invalidate( CACHE_PLAYER_DAMAGE_MULTIPLIER );

  buff.prepared =
    buff_creator_t( this, "prepared",
                    talent.prepared -> effectN( 1 ).trigger() )
      .trigger_spell( talent.prepared )
      .tick_callback( [this]( buff_t* b, int, const timespan_t& ) {
        resource_gain( RESOURCE_FURY, b -> data().effectN( 1 ).resource( RESOURCE_FURY ),
          gain.prepared );
      } );

  buff.rage_of_the_illidari =
    buff_creator_t( this, "rage_of_the_illidari", find_spell( 217060 ) );

  buff.vengeful_retreat_jump_cancel =
    buff_creator_t( this, "vengeful_retreat_jump_cancel",
                    spell_data_t::nil() )
      .chance( 1.0 )
      .duration( timespan_t::from_seconds( 1.25 ) );

  // Vengeance
  buff.defensive_spikes = 
    buff_creator_t( this, "defensive_spikes",
      artifact.defensive_spikes.data().effectN( 1 ).trigger() )
      .trigger_spell( artifact.defensive_spikes )
      .default_value( artifact.defensive_spikes.data().effectN( 1 ).trigger() ->
        effectN( 1 ).percent() )
      .add_invalidate( CACHE_PARRY );

  buff.demon_spikes =
    buff_creator_t( this, "demon_spikes", find_spell( 203819 ) )
      .default_value( find_spell( 203819 ) -> effectN( 1 ).percent() )
      .refresh_behavior( BUFF_REFRESH_EXTEND )
      .add_invalidate( CACHE_PARRY )
      .add_invalidate( CACHE_LEECH );

  buff.empower_wards =
    buff_creator_t( this, "empower_wards", find_specialization_spell( "Empower Wards" ) )
      .default_value( find_specialization_spell( "Empower Wards" ) -> effectN( 1 ).percent() );

  buff.immolation_aura =
    buff_creator_t( this, "immolation_aura", spec.immolation_aura )
      .tick_callback( [this]( buff_t*, int, const timespan_t& ) {
        active.immolation_aura -> schedule_execute();
        resource_gain( RESOURCE_PAIN, active.immolation_aura ->
          data().effectN( 2 ).resource( RESOURCE_PAIN ), gain.immolation_aura );
      } )
      .default_value( talent.agonizing_flames -> effectN( 2 ).percent() )
      .add_invalidate( CACHE_RUN_SPEED );

  buff.painbringer = 
    buff_creator_t( this, "painbringer", find_spell( 212988 ) )
      .trigger_spell( artifact.painbringer )
      .default_value( find_spell( 212988 ) -> effectN( 1 ).percent() )
      .stack_behavior( BUFF_STACK_ASYNCHRONOUS );

  buff.soul_barrier =
    absorb_buff_creator_t( this, "soul_barrier", find_spell( 211512 ) )
      .source( get_stats( "soul_barrier" ) )
      .gain( get_gain( "soul_barrier" ) )
      .high_priority( true ); // TOCHECK
}

// demon_hunter_t::create_expression ========================================

expr_t* demon_hunter_t::create_expression( action_t* a, const std::string& name_str )
{
  if ( name_str == "soul_fragments" )
  {
    return make_mem_fn_expr( name_str, *this, &demon_hunter_t::get_soul_fragments );
  }
  else if ( name_str == "soul_fragments_major" )
  {
    return make_ref_expr( name_str, soul_fragments );
  }
  else if ( name_str == "soul_fragments_lesser" )
  {
    return make_ref_expr( name_str, lesser_soul_fragments );
  }

  return player_t::create_expression( a, name_str );
}

// demon_hunter_t::create_pet ===============================================

pet_t* demon_hunter_t::create_pet( const std::string& pet_name,
                                   const std::string& /* pet_type */ )
{
  pet_t* p = find_pet( pet_name );

  if ( p )
    return p;

  // Add pets here

  sim -> errorf( "No demon hunter pet with name '%s' available.",
               pet_name.c_str() );

  return nullptr;
}

// demon_hunter_t::create_profile ===========================================

std::string demon_hunter_t::create_profile( save_e type )
{
  std::string profile_str = base_t::create_profile( type );

  // Log all options here

  return profile_str;
}

// demon_hunter_t::has_t18_class_trinket ====================================

bool demon_hunter_t::has_t18_class_trinket() const
{
  return false;
}

// demon_hunter_t::init_absorb_priority =====================================

void demon_hunter_t::init_absorb_priority()
{
  player_t::init_absorb_priority();

  absorb_priority.push_back( 211512 ); // Soul Barrier
}

// demon_hunter_t::init_action_list =========================================

void demon_hunter_t::init_action_list()
{
  if ( main_hand_weapon.type == WEAPON_NONE ||
       off_hand_weapon.type == WEAPON_NONE )
  {
    if ( !quiet )
    {
      sim -> errorf(
        "Player %s does not have a valid main-hand and off-hand weapon.",
        name() );
    }
    quiet = true;
    return;
  }

  if ( !action_list_str.empty() )
  {
    player_t::init_action_list();
    return;
  }
  clear_action_priority_lists();

  apl_precombat();  // PRE-COMBAT

  switch ( specialization() )
  {
    case DEMON_HUNTER_HAVOC:
      apl_havoc();
      break;
    case DEMON_HUNTER_VENGEANCE:
      apl_vengeance();
      break;
    default:
      apl_default();  // DEFAULT
      break;
  }

  use_default_action_list = true;

  base_t::init_action_list();
}

// demon_hunter_t::init_base_stats ==========================================

void demon_hunter_t::init_base_stats()
{
  base_t::init_base_stats();

  switch( specialization() )
  {
    case DEMON_HUNTER_HAVOC:
      resources.base[ RESOURCE_FURY ] = 100 + artifact.contained_fury.value();
      break;
    case DEMON_HUNTER_VENGEANCE:
      resources.base[ RESOURCE_PAIN ] = 100;
      break;
    default:
      break;
  }

  base.attack_power_per_strength = 0.0;
  base.attack_power_per_agility  = 1.0;

  // Avoidance diminishing Returns constants/conversions now handled in
  // player_t::init_base_stats().
  // Base miss, dodge, parry, and block are set in player_t::init_base_stats().
  // Just need to add class- or spec-based modifiers here.

  base_gcd = timespan_t::from_seconds( 1.5 );
}

// demon_hunter_t::init_procs ===============================================

void demon_hunter_t::init_procs()
{
  base_t::init_procs();

  // General
  proc.delayed_aa_range     = get_proc( "delayed_swing__out_of_range" );
  proc.delayed_aa_channel   = get_proc( "delayed_swing__channeling" );
  proc.soul_fragment        = get_proc( "soul_fragment" );
  proc.soul_fragment_lesser = get_proc( "soul_fragment_lesser" );

  // Havoc
  proc.demonic_appetite     = get_proc( "demonic_appetite" );
  proc.demons_bite_in_meta  = get_proc( "demons_bite_in_meta" );
  proc.felblade_reset       = get_proc( "felblade_reset" );
  proc.fel_barrage          = get_proc( "fel_barrage" );

  // Vengeance
  proc.fueled_by_pain       = get_proc( "fueled_by_pain" );
}

// demon_hunter_t::init_resources ===========================================

void demon_hunter_t::init_resources( bool force )
{
  base_t::init_resources( force );

  if ( artifact.will_of_the_illidari.rank() )
  {
    recalculate_resource_max( RESOURCE_HEALTH );
    resources.initial[ RESOURCE_HEALTH ] = resources.current[ RESOURCE_HEALTH ]
      = resources.max[ RESOURCE_HEALTH ];
  }

  resources.current[ RESOURCE_FURY ] = 0;
  resources.current[ RESOURCE_PAIN ] = 0;
}

// demon_hunter_t::init_rng =================================================

void demon_hunter_t::init_rng()
{
  // RPPM objects

  // General
  rppm.felblade = get_rppm( "felblade", find_spell( 203557 ) );

  // Havoc
  rppm.demon_blades = get_rppm( "demon_blades", talent.demon_blades );
  rppm.inner_demons = get_rppm( "inner_demons", artifact.inner_demons );

  // Vengeance
  rppm.fueled_by_pain = get_rppm( "fueled_by_pain", artifact.fueled_by_pain );

  player_t::init_rng();
}

// demon_hunter_t::init_scaling =============================================

void demon_hunter_t::init_scaling()
{
  base_t::init_scaling();

  scales_with[ STAT_WEAPON_OFFHAND_DPS ] = true;

  if ( specialization() == DEMON_HUNTER_VENGEANCE )
    scales_with[ STAT_BONUS_ARMOR ] = true;

  scales_with[ STAT_STRENGTH ] = false;
}

// demon_hunter_t::init_spells ==============================================

void demon_hunter_t::init_spells()
{
  base_t::init_spells();

  // Specialization =========================================================

  // General
  spec.demon_hunter           = find_class_spell( "Demon Hunter" );
  spec.consume_magic          = find_class_spell( "Consume Magic" );
  spec.consume_soul           = find_spell( 210042 );
  spec.consume_soul_lesser    = find_spell( 203794 );
  spec.critical_strikes       = find_spell( 221351 );  // not a class spell
  spec.leather_specialization = find_spell( 178976 );
  spec.metamorphosis_buff     = specialization() == DEMON_HUNTER_HAVOC ?
    find_spell( 162264 ) : find_spell( 187827 );

  // Havoc
  spec.havoc              = find_specialization_spell( "Havoc Demon Hunter" );
  spec.annihilation       = find_spell( 201427 );
  spec.blade_dance        = find_class_spell( "Blade Dance" );
  spec.blur               = find_class_spell( "Blur" );
  spec.chaos_nova         = find_class_spell( "Chaos Nova" );
  spec.chaos_strike       = find_class_spell( "Chaos Strike" );
  spec.death_sweep        = find_spell( 210152 );

  // Vengeance
  spec.vengeance          = find_specialization_spell( "Vengeance Demon Hunter" );
  spec.demonic_wards      = find_specialization_spell( "Demonic Wards" );
  spec.fiery_brand_dr     = find_spell( 209245 );
  spec.immolation_aura    = find_specialization_spell( "Immolation Aura" );
  spec.riposte            = find_specialization_spell( "Riposte" );
  spec.soul_cleave        = find_specialization_spell( "Soul Cleave" );

  // Masteries ==============================================================

  mastery_spell.demonic_presence = find_mastery_spell( DEMON_HUNTER_HAVOC );
  mastery_spell.fel_blood        = find_mastery_spell( DEMON_HUNTER_VENGEANCE );

  // Talents ================================================================

  // General
  talent.fel_eruption         = find_talent_spell( "Fel Eruption" );

  talent.felblade             = find_talent_spell( "Felblade" );

  talent.soul_rending         = find_talent_spell( "Soul Rending" );

  // Havoc
  talent.fel_mastery          = find_talent_spell( "Fel Mastery" );
  talent.demonic_appetite     = find_talent_spell( "Demonic Appetite" );
  talent.blind_fury           = find_talent_spell( "Blind Fury" );

  talent.prepared             = find_talent_spell( "Prepared" );
  talent.chaos_cleave         = find_talent_spell( "Chaos Cleave" );

  talent.first_blood          = find_talent_spell( "First Blood" );
  talent.bloodlet             = find_talent_spell( "Bloodlet" );

  talent.netherwalk           = find_talent_spell( "Netherwalk" );
  talent.desperate_instincts  = find_talent_spell( "Desperate Instincts" );

  talent.momentum             = find_talent_spell( "Momentum" );
  talent.demonic              = find_talent_spell( "Demonic" );
  talent.nemesis              = find_talent_spell( "Nemesis" );

  talent.master_of_the_glaive = find_talent_spell( "Master of the Glaive" );
  talent.unleashed_power      = find_talent_spell( "Unleashed Power" );
  talent.demon_blades         = find_talent_spell( "Demon Blades" );

  talent.chaos_blades         = find_talent_spell( "Chaos Blades" );
  talent.fel_barrage          = find_talent_spell( "Fel Barrage" );
  talent.demon_reborn         = find_talent_spell( "Demon Reborn" );

  // Vengeance
  talent.abyssal_strike       = find_talent_spell( "Abyssal Strike" );
  talent.blade_turning        = find_talent_spell( "Blade Turning" );
  talent.fracture             = find_talent_spell( "Fracture" );
    
  talent.agonizing_flames     = find_talent_spell( "Agonizing Flames" );
  talent.quickened_sigils     = find_talent_spell( "Quickened Sigils" );

  talent.soul_barrier         = find_talent_spell( "Soul Barrier" );
  talent.last_resort          = find_talent_spell( "Last Resort" );

  talent.spirit_bomb          = find_talent_spell( "Spirit Bomb" );
  talent.etched_in_blood      = find_talent_spell( "Etched in Blood" );
    
  talent.burning_alive        = find_talent_spell( "Burning Alive" );
  talent.concentrated_sigils  = find_talent_spell( "Concentrated Sigils" );
  talent.feast_of_souls       = find_talent_spell( "Feast of Souls" );

  talent.brand_of_the_hunt    = find_talent_spell( "Brand of the Hunt" );
  talent.sigil_of_misery      = find_talent_spell( "Sigil of Misery" );
  talent.razor_spikes         = find_talent_spell( "Razor Spikes" );

  talent.fel_devastation      = find_talent_spell( "Fel Devastation" );
  talent.nether_bond          = find_talent_spell( "Nether Bond" );
  talent.feed_the_demon       = find_talent_spell( "Feed the Demon" );

  // Artifacts ==============================================================

  // Havoc -- Twinblades of the Deceiver
  artifact.anguish_of_the_deceiver = find_artifact_spell( "Anguish of the Deceiver" );
  artifact.balanced_blades         = find_artifact_spell( "Balanced Blades" );
  artifact.chaos_vision            = find_artifact_spell( "Chaos Vision" );
  artifact.contained_fury          = find_artifact_spell( "Contained Fury" );
  artifact.critical_chaos          = find_artifact_spell( "Critical Chaos" );
  artifact.deceivers_fury          = find_artifact_spell( "Deceiver's Fury" );
  artifact.demon_rage              = find_artifact_spell( "Demon Rage" );
  artifact.demon_speed             = find_artifact_spell( "Demon Speed" );
  artifact.feast_on_the_souls      = find_artifact_spell( "Feast on the Souls" );
  artifact.fury_of_the_illidari    = find_artifact_spell( "Fury of the Illidari" );
  artifact.illidari_knowledge      = find_artifact_spell( "Illidari Knowledge" );
  artifact.inner_demons            = find_artifact_spell( "Inner Demons" );
  artifact.overwhelming_power      = find_artifact_spell( "Overwhelming Power" );
  artifact.rage_of_the_illidari    = find_artifact_spell( "Rage of the Illidari" );
  artifact.sharpened_glaives       = find_artifact_spell( "Sharpened Glaives" );
  artifact.unleashed_demons        = find_artifact_spell( "Unleashed Demons" );
  artifact.warglaives_of_chaos     = find_artifact_spell( "Warglaives of Chaos" );

  // Vengeance -- The Aldrachi Warblades
  artifact.aldrachi_design         = find_artifact_spell( "Aldrachi Design" );
  artifact.aura_of_pain            = find_artifact_spell( "Aura of Pain" );
  artifact.charred_warblades       = find_artifact_spell( "Charred Warblades" );
  artifact.defensive_spikes        = find_artifact_spell( "Defensive Spikes" );
  artifact.demonic_flames          = find_artifact_spell( "Demonic Flames" );
  artifact.devour_souls            = find_artifact_spell( "Devour Souls" );
  artifact.embrace_the_pain        = find_artifact_spell( "Embrace the Pain" );
  artifact.fiery_demise            = find_artifact_spell( "Fiery Demise" );
  artifact.fueled_by_pain          = find_artifact_spell( "Fueled by Pain" );
  artifact.honed_warblades         = find_artifact_spell( "Honed Warblades" );
  artifact.infernal_force          = find_artifact_spell( "Infernal Force" );
  artifact.painbringer             = find_artifact_spell( "Painbringer" );
  artifact.shatter_the_souls       = find_artifact_spell( "Shatter the Souls" );
  artifact.siphon_power            = find_artifact_spell( "Siphon Power" );
  artifact.soul_carver             = find_artifact_spell( "Soul Carver" );
  artifact.tormented_souls         = find_artifact_spell( "Tormented Souls" );
  artifact.will_of_the_illidari    = find_artifact_spell( "Will of the Illidari" );

  // Spell Initialization ===================================================

  if ( talent.demonic_appetite -> ok() )
  {
    spec.demonic_appetite_fury = find_spell( 210041 );
    cooldown.demonic_appetite -> duration =
      talent.demonic_appetite -> internal_cooldown();
  }

  if ( talent.fel_barrage -> ok() )
  {
    spec.fel_barrage_proc = find_spell( 222703 );
    cooldown.fel_barrage_proc -> duration =
      spec.fel_barrage_proc -> internal_cooldown();
  }

  using namespace actions::attacks;
  using namespace actions::spells;
  using namespace actions::heals;

  active.consume_soul =
    new consume_soul_t( this, "consume_soul", spec.consume_soul, &soul_fragments );
  active.consume_soul_lesser =
    new consume_soul_t( this, "consume_soul_lesser", spec.consume_soul_lesser,
      &lesser_soul_fragments );

  if ( spec.blade_dance -> ok() )
  {
    blade_dance_attacks.push_back( new blade_dance_attack_t(
      this, spec.blade_dance -> effectN( 4 ).trigger(), "blade_dance1" ) );
    blade_dance_attacks.push_back( new blade_dance_attack_t(
      this, spec.blade_dance -> effectN( 5 ).trigger(), "blade_dance2" ) );
    blade_dance_attacks.push_back( new blade_dance_attack_t(
      this, spec.blade_dance -> effectN( 6 ).trigger(), "blade_dance3" ) );
    blade_dance_attacks.push_back( new blade_dance_attack_t(
      this, spec.blade_dance -> effectN( 7 ).trigger(), "blade_dance4" ) );
  }

  if ( spec.death_sweep -> ok() )
  {
    death_sweep_attacks.push_back( new blade_dance_attack_t(
      this, spec.death_sweep -> effectN( 4 ).trigger(), "death_sweep1" ) );
    death_sweep_attacks.push_back( new blade_dance_attack_t(
      this, spec.death_sweep -> effectN( 5 ).trigger(), "death_sweep2" ) );
    death_sweep_attacks.push_back( new blade_dance_attack_t(
      this, spec.death_sweep -> effectN( 6 ).trigger(), "death_sweep3" ) );
    death_sweep_attacks.push_back( new blade_dance_attack_t(
      this, spec.death_sweep -> effectN( 7 ).trigger(), "death_sweep4" ) );
  }

  if ( talent.chaos_blades -> ok() )
  {
    chaos_blade_main_hand = new chaos_blade_t(
      "chaos_blade_mh", this,
      find_spell( talent.chaos_blades -> effectN( 1 ).misc_value1() ) );

    chaos_blade_off_hand = new chaos_blade_t(
      "chaos_blade_oh", this, talent.chaos_blades -> effectN( 1 ).trigger() );
  }

  if ( talent.demon_blades -> ok() )
  {
    active.demon_blades = new demon_blades_t( this );
  }

  if ( artifact.inner_demons.rank() )
  {
    active.inner_demons = new inner_demons_t( this );
  }

  if ( artifact.anguish_of_the_deceiver.rank() )
  {
    active.anguish = new anguish_t( this );
  }

  if ( spec.immolation_aura -> ok() )
  {
    active.immolation_aura = new immolation_aura_damage_t( this,
      spec.immolation_aura -> effectN( 1 ).trigger() );
    active.immolation_aura -> stats = get_stats( "immolation_aura" );
  }

  if ( artifact.charred_warblades.rank() )
  {
    active.charred_warblades = new charred_warblades_t( this );
  }

  // Legendary
  spec.fragment_of_the_betrayers_prison = find_spell( 217500 );
}

// demon_hunter_t::invalidate_cache =========================================

void demon_hunter_t::invalidate_cache( cache_e c )
{
  player_t::invalidate_cache( c );

  switch ( c )
  {
    case CACHE_MASTERY:
      if ( mastery_spell.demonic_presence -> ok() )
      {
        invalidate_cache( CACHE_RUN_SPEED );
      }
      break;
    case CACHE_CRIT:
      if ( spec.riposte -> ok() )
      {
        invalidate_cache( CACHE_PARRY );
      }
    default:
      break;
  }
}

// demon_hunter_t::primary_resource =========================================

resource_e demon_hunter_t::primary_resource() const
{
  switch ( specialization() )
  {
    case DEMON_HUNTER_HAVOC:
      return RESOURCE_FURY;
    case DEMON_HUNTER_VENGEANCE:
      return RESOURCE_PAIN;
    default:
      return RESOURCE_NONE;
  }
}

// demon_hunter_t::primary_role =============================================

role_e demon_hunter_t::primary_role() const
{
  switch ( specialization() )
  {
    case DEMON_HUNTER_HAVOC:
      return ROLE_ATTACK;
    case DEMON_HUNTER_VENGEANCE:
      return ROLE_TANK;
    default:
      return ROLE_NONE;
  }
}

// ==========================================================================
// custom demon_hunter_t init functions
// ==========================================================================

// demon_hunter_t::apl_precombat ============================================

void demon_hunter_t::apl_precombat()
{
  action_priority_list_t* pre = get_action_priority_list( "precombat" );

  // Flask or Elixir
  if ( sim -> allow_flasks )
  {
    if ( true_level > 100 )
      pre -> add_action( "flask,type=flask_of_the_seventh_demon" );
    else 
      pre -> add_action( "flask,type=greater_draenic_agility_flask" );
  }

  // Food
  if ( sim -> allow_food )
  {
    if ( true_level > 100 )
      pre -> add_action( "food,type=the_hungry_magister" );
    else
      pre -> add_action( "food,type=pickled_eel" );
  }

  // Snapshot Stats
  pre -> add_action( "snapshot_stats", "Snapshot raid buffed stats before combat begins and pre-potting is done." );

  // Pre-Potion
  if ( sim -> allow_potions )
  {
    /* if ( true_level > 100 )
      pre -> add_action( "potion,name=potion_of_the_old_war" );
    else */
      pre -> add_action( "potion,name=draenic_agility_potion" );
  }
}

// demon_hunter_t::apl_default ==============================================

void demon_hunter_t::apl_default()
{
  // action_priority_list_t* def = get_action_priority_list( "default" );
}

// demon_hunter_t::apl_havoc ================================================

void demon_hunter_t::apl_havoc()
{
  talent_overrides_str += "/blind_fury,if=desired_targets+raid_event.adds.count>=4";
  talent_overrides_str += "/chaos_cleave,if=desired_targets>1|raid_event.adds.exists";

  action_priority_list_t* def = get_action_priority_list( "default" );

  def -> add_action( "auto_attack" );
  def -> add_talent( this, "Nemesis", "target_if=min:target.time_to_die,if=active_enemies>desired_targets|raid_event.adds.in>60" );
  def -> add_action( this, "Consume Magic" );
  def -> add_action( this, "Vengeful Retreat", "jump_cancel=1,if=gcd.remains&((talent.momentum.enabled&buff.momentum.down)|!talent.momentum.enabled)" );
  def -> add_action( this, "Fel Rush", "jump_cancel=1,if=talent.momentum.enabled&buff.momentum.down&raid_event.movement.in>charges*10&(charges=2|cooldown.vengeful_retreat.remains>4)" );
  def -> add_action( this, "Eye Beam", "if=talent.demonic.enabled&buff.metamorphosis.down&(!talent.first_blood.enabled|fury>=80|fury.deficit<30)" );
  def -> add_talent( this, "Chaos Blades", "if=buff.chaos_blades.down&cooldown.metamorphosis.remains>100" );
  def -> add_talent( this, "Chaos Blades", "sync=metamorphosis" );
  def -> add_action( this, "Metamorphosis", "if=buff.metamorphosis.down&(!talent.demonic.enabled|!cooldown.eye_beam.ready)&(cooldown.chaos_blades.ready|buff.chaos_blades.up|!talent.demon_reborn.enabled)" );
  def -> add_action( this, spec.death_sweep, "death_sweep", "if=talent.first_blood.enabled|spell_targets>1+talent.chaos_cleave.enabled" );
  def -> add_action( this, "Demon's Bite", "if=buff.metamorphosis.remains>gcd&(talent.first_blood.enabled|spell_targets>1+talent.chaos_cleave.enabled)&cooldown.blade_dance.remains<gcd&fury<70" );
  def -> add_action( this, "Blade Dance", "if=talent.first_blood.enabled|spell_targets>1+talent.chaos_cleave.enabled" );
  def -> add_talent( this, "Fel Barrage", "if=charges>=5" );
  def -> add_action( this, artifact.fury_of_the_illidari, "fury_of_the_illidari", "if=buff.metamorphosis.down|spell_targets.fury_of_the_illidari_tick>2+talent.chaos_cleave.enabled" );
  def -> add_action( this, "Throw Glaive", "if=talent.bloodlet.enabled&spell_targets>=2+talent.chaos_cleave.enabled" );
  def -> add_talent( this, "Felblade", "if=fury.deficit>=30+buff.prepared.up*12" );
  def -> add_action( this, spec.annihilation, "annihilation" );
  def -> add_talent( this, "Fel Eruption" );
  def -> add_action( this, "Throw Glaive", "if=buff.metamorphosis.down&talent.bloodlet.enabled" );
  def -> add_action( this, "Eye Beam", "if=!talent.demonic.enabled&(spell_targets.eye_beam_tick>desired_targets|raid_event.adds.in>45)" );
  def -> add_action( this, "Demon's Bite", "if=buff.metamorphosis.down&(talent.first_blood.enabled|spell_targets>1+talent.chaos_cleave.enabled)&cooldown.blade_dance.remains<gcd&fury<55" );
  def -> add_action( this, "Demon's Bite", "if=talent.demonic.enabled&buff.metamorphosis.down&cooldown.eye_beam.remains<gcd&fury.deficit>=30" );
  def -> add_action( this, "Demon's Bite", "if=talent.demonic.enabled&buff.metamorphosis.down&cooldown.eye_beam.remains<2*gcd&fury.deficit>=55" );
  def -> add_action( this, "Throw Glaive", "if=buff.metamorphosis.down&(talent.bloodlet.enabled|spell_targets>=3)" );
  def -> add_action( this, "Chaos Strike" );
  def -> add_talent( this, "Fel Barrage", "if=charges=4&buff.metamorphosis.down" );
  def -> add_action( this, "Fel Rush", "jump_cancel=1,if=!talent.momentum.enabled&raid_event.movement.in>charges*10" );
  def -> add_action( this, "Demon's Bite" );
  def -> add_action( this, "Fel Rush", "if=movement.distance" );
  def -> add_action( this, "Vengeful Retreat", "if=movement.distance" );
}

// demon_hunter_t::apl_vengeance ============================================

void demon_hunter_t::apl_vengeance()
{
  action_priority_list_t* def = get_action_priority_list( "default" );

  def -> add_action( "auto_attack" );
  def -> add_action( this, "Fiery Brand" );
  def -> add_action( this, "Demon Spikes" );
  def -> add_action( this, "Empower Wards", "if=debuff.casting.up" );
  def -> add_action( this, "Soul Cleave", "if=incoming_damage_3s>health.max*0.25" );
  def -> add_action( this, "Immolation Aura" );
  def -> add_talent( this, "Fracture", "if=pain>=80&incoming_damage_6s=0" );
  def -> add_action( this, "Soul Cleave", "if=pain>=80" );
  def -> add_talent( this, "Felblade" );
  def -> add_action( this, "Sigil of Flame" );
  def -> add_talent( this, "Fel Eruption" );
  def -> add_talent( this, "Spirit Bomb", "if=debuff.frail.down" );
  def -> add_talent( this, "Fel Devastation" );
  def -> add_action( this, artifact.soul_carver, "soul_carver" );
  def -> add_action( this, "Shear" );
}

// demon_hunter_t::create_cooldowns =========================================

void demon_hunter_t::create_cooldowns()
{
  // General
  cooldown.consume_magic        = get_cooldown( "consume_magic" );
  cooldown.felblade             = get_cooldown( "felblade" );
  cooldown.fel_eruption         = get_cooldown( "fel_eruption" );
  
  // Havoc
  cooldown.blade_dance          = get_cooldown( "blade_dance" );
  cooldown.blur                 = get_cooldown( "blur" );
  cooldown.chaos_blades         = get_cooldown( "chaos_blades" );
  cooldown.chaos_nova           = get_cooldown( "chaos_nova" );
  cooldown.death_sweep          = get_cooldown( "death_sweep" );
  cooldown.demonic_appetite     = get_cooldown( "demonic_appetite" );
  cooldown.eye_beam             = get_cooldown( "eye_beam" );
  cooldown.fel_barrage          = get_cooldown( "fel_barrage" );
  cooldown.fel_barrage_proc     = get_cooldown( "fel_barrage_proc" );
  cooldown.fel_rush             = get_cooldown( "fel_rush" );
  cooldown.fury_of_the_illidari = get_cooldown( "fury_of_the_illidari" );
  cooldown.nemesis              = get_cooldown( "nemesis" );
  cooldown.netherwalk           = get_cooldown( "netherwalk" );
  cooldown.throw_glaive         = get_cooldown( "throw_glaive" );
  cooldown.vengeful_retreat     = get_cooldown( "vengeful_retreat" );

  // Vengeance
  cooldown.demon_spikes         = get_cooldown( "demon_spikes" );
  cooldown.fiery_brand          = get_cooldown( "fiery_brand" );
  cooldown.sigil_of_chains      = get_cooldown( "sigil_of_chains" );
  cooldown.sigil_of_flame       = get_cooldown( "sigil_of_flame" );
  cooldown.sigil_of_misery      = get_cooldown( "sigil_of_misery" );
  cooldown.sigil_of_silence     = get_cooldown( "sigil_of_silence" );

  
  if ( legendary.the_defilers_lost_vambraces )
  {
    sigil_cooldowns.push_back( cooldown.sigil_of_chains );
    sigil_cooldowns.push_back( cooldown.sigil_of_flame );
    sigil_cooldowns.push_back( cooldown.sigil_of_silence );
    if ( talent.sigil_of_misery -> ok() )
    {
      sigil_cooldowns.push_back( cooldown.sigil_of_misery );
    }
  }
}

// demon_hunter_t::create_gains =============================================

void demon_hunter_t::create_gains()
{
  // General
  gain.miss_refund      = get_gain( "miss_refund" );

  // Havoc
  gain.demonic_appetite = get_gain( "demonic_appetite" );
  gain.prepared         = get_gain( "prepared" );
  
  // Vengeance
  gain.blade_turning    = get_gain( "blade_turning" );
  gain.immolation_aura  = get_gain( "immolation_aura" );
  gain.metamorphosis    = get_gain( "metamorphosis" );
}

// demon_hunter_t::create_benefits ==========================================

void demon_hunter_t::create_benefits() {}

// ==========================================================================
// overridden player_t stat functions
// ==========================================================================

// demon_hunter_t::composite_armor_multiplier ===============================

double demon_hunter_t::composite_armor_multiplier() const
{
  double am = player_t::composite_armor_multiplier();

  am *= 1.0 + spec.demonic_wards -> effectN( 5 ).percent();

  return am;
}

// demon_hunter_t::composite_attack_power_multiplier ========================

double demon_hunter_t::composite_attack_power_multiplier() const
{
  double ap = player_t::composite_attack_power_multiplier();

  ap *= 1.0 + cache.mastery() * mastery_spell.fel_blood -> effectN( 2 ).mastery_value();

  return ap;
}

// demon_hunter_t::composite_attribute_multiplier ===========================

double demon_hunter_t::composite_attribute_multiplier( attribute_e a ) const
{
  double am = player_t::composite_attribute_multiplier( a );

  switch( a )
  {
    case ATTR_STAMINA:
      am *= 1.0 + spec.demonic_wards -> effectN( 4 ).percent();
      break;
    default:
      break;
  }

  return am;
}

// demon_hunter_t::composite_crit_avoidance =================================

double demon_hunter_t::composite_crit_avoidance() const
{
  double ca = player_t::composite_crit_avoidance();

  ca += spec.demonic_wards -> effectN( 2 ).percent();

  return ca;
}

// demon_hunter_t::composite_dodge ==========================================

double demon_hunter_t::composite_dodge() const
{
  double d = player_t::composite_dodge();

  d += buff.blade_dance -> check_value();
  d += buff.death_sweep -> check_value();

  d += buff.blur -> check() * buff.blur -> data().effectN( 2 ).percent();

  return d;
}

// demon_hunter_t::composite_leech ==========================================

double demon_hunter_t::composite_leech() const
{
  double l = player_t::composite_leech();

  if ( talent.soul_rending -> ok() && buff.metamorphosis -> check() )
  {
    l += talent.soul_rending -> effectN( 1 ).percent();
  }

  if ( legendary.eternal_hunger && buff.blur -> check() )
  {
    l += legendary.eternal_hunger -> driver() -> effectN( 1 ).percent();
  }

  if ( buff.demon_spikes -> check() && legendary.fragment_of_the_betrayers_prison ) // legendary effect
  {
    l += spec.fragment_of_the_betrayers_prison -> effectN( 1 ).percent();
  }

  return l;
}

// demon_hunter_t::composite_melee_crit =====================================

double demon_hunter_t::composite_melee_crit() const
{
  double mc = player_t::composite_melee_crit();

  mc += spec.critical_strikes -> effectN( 1 ).percent();

  return mc;
}

// demon_hunter_t::composite_melee_expertise ================================

double demon_hunter_t::composite_melee_expertise( const weapon_t* w ) const
{
  double me = player_t::composite_melee_expertise( w );

  me += spec.demonic_wards -> effectN( 3 ).base_value();

  return me;
}

// demon_hunter_t::composite_parry ==========================================

double demon_hunter_t::composite_parry() const
{
  double cp = player_t::composite_parry();

  cp += buff.demon_spikes -> check_value();

  cp += artifact.aldrachi_design.percent();

  cp += buff.defensive_spikes -> check_value();

  return cp;
}

// demon_hunter_t::composite_parry_rating() =================================

double demon_hunter_t::composite_parry_rating() const
{
  double pr = player_t::composite_parry_rating();

  if ( spec.riposte -> ok() )
    pr += composite_melee_crit_rating();

  return pr;
}

// demon_hunter_t::composite_player_multiplier ==============================

double demon_hunter_t::composite_player_multiplier( school_e school ) const
{
  double m = player_t::composite_player_multiplier( school );

  m *= 1.0 + buff.momentum -> check_value();

  // TODO: Figure out how to access target's race.
  m *= 1.0 + buff.nemesis -> check() * buff.nemesis -> data().effectN( 1 ).percent();

  m *= 1.0 + buff.chaos_blades -> check() * cache.mastery_value();

  return m;
}

// demon_hunter_t::composite_spell_crit =====================================

double demon_hunter_t::composite_spell_crit() const
{
  double sc = player_t::composite_spell_crit();

  sc += spec.critical_strikes -> effectN( 1 ).percent();

  return sc;
}

// demon_hunter_t::matching_gear_multiplier =================================

double demon_hunter_t::matching_gear_multiplier( attribute_e attr ) const
{
  if ( spec.leather_specialization -> ok()
    && stat_from_attr( attr ) == STAT_AGILITY )
  {
    return spec.leather_specialization -> effectN( 1 ).percent();
  }

  return 0.0;
}

// demon_hunter_t::passive_movement_modifier ================================

double demon_hunter_t::passive_movement_modifier() const
{
  double ms = player_t::passive_movement_modifier();

  if ( mastery_spell.demonic_presence -> ok() )
    ms += cache.mastery() *
          mastery_spell.demonic_presence -> effectN( 2 ).mastery_value();

  ms += buff.immolation_aura -> value();

  return ms;
}

// demon_hunter_t::temporary_movement_modifier ==============================

double demon_hunter_t::temporary_movement_modifier() const
{
  double ms = player_t::temporary_movement_modifier();

  ms += std::max( ms, buff.immolation_aura -> value() );

  return ms;
}

// ==========================================================================
// overridden player_t combat functions
// ==========================================================================

// demon_hunter_t::assess_damage ============================================

void demon_hunter_t::assess_damage( school_e school, dmg_e dt,
                                        action_state_t* s )
{
  player_t::assess_damage( school, dt, s );

  if ( talent.blade_turning -> ok() && s -> result == RESULT_PARRY )
  {
    resource_gain( RESOURCE_PAIN, talent.blade_turning -> effectN( 1 ).trigger() ->
      effectN( 1 ).resource( RESOURCE_PAIN ), gain.blade_turning );
  }

  // Benefit tracking
  if ( s -> action -> may_parry )
  {
    buff.demon_spikes -> up();
    buff.defensive_spikes -> up();
  }
}

// demon_hunter_t::interrupt ================================================

void demon_hunter_t::interrupt()
{
  if ( blade_dance_driver )
  {
    event_t::cancel( blade_dance_driver );
  }

  base_t::interrupt();
}

// demon_hunter_t::recalculate_resource_max =================================

void demon_hunter_t::recalculate_resource_max( resource_e r )
{
  player_t::recalculate_resource_max( r );
  
  if ( r == RESOURCE_HEALTH )
  {
    resources.max[ r ] *= 1.0 + artifact.will_of_the_illidari.percent();

    // Update Metamorphosis' value for the new health amount.
    if ( buff.metamorphosis -> check() )
    {
      assert( metamorphosis_health > 0 );

      double base_health = max_health() - metamorphosis_health;
      double new_health = base_health * buff.metamorphosis -> check_value();
      double diff = new_health - metamorphosis_health;

      if ( diff != 0.0 )
      {
        if ( sim -> debug )
        {
          sim -> out_debug.printf( "%s adjusts %s temporary health. old=%.0f new=%.0f diff=%.0f",
            name(), buff.metamorphosis -> name(), metamorphosis_health, new_health, diff );
        }
    
        resources.max[ RESOURCE_HEALTH ] += diff;
        resources.temporary[ RESOURCE_HEALTH ] += diff;
        if ( diff > 0 )
        {
          resource_gain( RESOURCE_HEALTH, diff );
        }
        else if ( diff < 0 )
        {
          resource_loss( RESOURCE_HEALTH, -diff );
        }

        metamorphosis_health += diff;
      }
    }
  }
}

// demon_hunter_t::reset ====================================================

void demon_hunter_t::reset()
{
  base_t::reset();

  blade_dance_driver = nullptr;
  soul_fragments = 0;
  lesser_soul_fragments = 0;
  shear_counter = 0;
  metamorphosis_health = 0;
}

// demon_hunter_t::target_mitigation ========================================

void demon_hunter_t::target_mitigation( school_e school, dmg_e dt,
                                        action_state_t* s )
{
  base_t::target_mitigation( school, dt, s );

  if ( s -> result_amount <= 0 )
  {
    return;
  }

  s -> result_amount *= 1.0 + buff.blur -> value();

  s -> result_amount *= 1.0 + buff.painbringer -> stack_value();

  if ( dbc::is_school( school, SCHOOL_PHYSICAL ) && buff.demon_spikes -> up() )
  {
    s -> result_amount *= 1.0 + buff.demon_spikes -> data().effectN( 2 ).percent() - cache.mastery_value();
  }

  if ( dbc::get_school_mask( school ) & SCHOOL_MAGIC_MASK )
  {
    // TOCHECK: Tooltip says magical damage but spell data says all damage.
    s -> result_amount *= 1.0 + spec.demonic_wards -> effectN( 1 ).percent();

    s -> result_amount *= 1.0 + buff.empower_wards -> value();

    if ( legendary.cloak_of_fel_flames && buff.immolation_aura -> check() )
    {
      s -> result_amount *= 1.0 - legendary.cloak_of_fel_flames -> driver() -> effectN( 1 ).percent();
    }
  }

  if ( get_target_data( s -> action -> player ) -> dots.fiery_brand -> is_ticking() )
  {
    s -> result_amount *= 1.0 + spec.fiery_brand_dr -> effectN( 2 ).percent();
  }
}

// ==========================================================================
// custom demon_hunter_t functions
// ==========================================================================

// demon_hunter_t::get_total_soul_fragments =================================

double demon_hunter_t::get_soul_fragments()
{ return soul_fragments + lesser_soul_fragments; }

// demon_hunter_t::spawn_soul_fragment ======================================

void demon_hunter_t::spawn_soul_fragment( unsigned n )
{
  soul_fragments += n;

  for ( unsigned i = 0; i < n; i++ )
  {
    proc.soul_fragment -> occur();
  }

  if ( sim -> debug )
  {
    sim -> out_debug.printf( "%s spawns %u soul fragments. count=%u", name(), n, soul_fragments );
  }
}

// demon_hunter_t::spawn_soul_fragment_lesser ===============================

void demon_hunter_t::spawn_soul_fragment_lesser( unsigned n )
{
  lesser_soul_fragments += n;

  for ( unsigned i = 0; i < n; i++ )
  {
    proc.soul_fragment_lesser -> occur();
  }

  if ( sim -> debug )
  {
    sim -> out_debug.printf( "%s spawns %u lesser soul fragments. count=%u", name(), n, lesser_soul_fragments );
  }
}

// demon_hunter_t::consume_soul_fragment ====================================

bool demon_hunter_t::consume_soul_fragments()
{
  if ( soul_fragments + lesser_soul_fragments == 0 )
  {
    return false;
  }

  if ( sim -> debug && soul_fragments > 0 )
  {
    sim -> out_debug.printf( "%s consumes %u soul fragments.", name(), soul_fragments );
  }

  for ( unsigned i = 0; i < soul_fragments; i++ )
  {
    active.consume_soul -> schedule_execute();
  }

  if ( sim -> debug && lesser_soul_fragments > 0 )
  {
    sim -> out_debug.printf( "%s consumes %u lesser soul fragments.", name(), lesser_soul_fragments );
  }

  for ( unsigned i = 0; i < lesser_soul_fragments; i++ )
  {
    active.consume_soul_lesser -> schedule_execute();
  }

  return true;
}

/* Always returns non-null targetdata pointer
 */
demon_hunter_td_t* demon_hunter_t::get_target_data( player_t* target ) const
{
  auto& td = _target_data[ target ];
  if ( !td )
  {
    td = new demon_hunter_td_t( target, const_cast<demon_hunter_t&>( *this ) );
  }
  return td;
}

/* Report Extension Class
 * Here you can define class specific report extensions/overrides
 */
class demon_hunter_report_t : public player_report_extension_t
{
public:
  demon_hunter_report_t( demon_hunter_t& player ) : p( player )
  {
  }

  void html_customsection( report::sc_html_stream& /* os*/ ) override
  {
    ( void ) p;
    /*// Custom Class Section
    os << "\t\t\t\t<div class=\"player-section custom_section\">\n"
        << "\t\t\t\t\t<h3 class=\"toggle open\">Custom Section</h3>\n"
        << "\t\t\t\t\t<div class=\"toggle-content\">\n";

    os << p.name();

    os << "\t\t\t\t\t\t</div>\n" << "\t\t\t\t\t</div>\n";*/
  }

private:
  demon_hunter_t& p;
};

static void init_special_effect( demon_hunter_t*          player,
                             specialization_e         spec,
                             const special_effect_t*& ptr,
                             const special_effect_t&  effect )
{
  // Ensure we have the spell data. This will prevent the trinket effect from working on live
  // Simulationcraft. Also ensure correct specialization.
  if ( ! player -> find_spell( effect.spell_id ) -> ok() ||
       ( player -> specialization() != spec && spec != SPEC_NONE ) )
  {
    return;
  }
  // Set pointer, module considers non-null pointer to mean the effect is "enabled"
  ptr = &( effect );
}

static void anger_of_the_halfgiants( special_effect_t& effect )
{
  demon_hunter_t* s = debug_cast<demon_hunter_t*>( effect.player );
  init_special_effect( s, DEMON_HUNTER_HAVOC, s -> legendary.anger_of_the_halfgiants, effect );
}

static void cloak_of_fel_flames( special_effect_t& effect )
{
  demon_hunter_t* s = debug_cast<demon_hunter_t*>( effect.player );
  init_special_effect( s, DEMON_HUNTER_VENGEANCE, s -> legendary.cloak_of_fel_flames, effect );
}

static void eternal_hunger( special_effect_t& effect )
{
  demon_hunter_t* s = debug_cast<demon_hunter_t*>( effect.player );
  init_special_effect( s, DEMON_HUNTER_HAVOC, s -> legendary.eternal_hunger, effect );
}

static void fragment_of_the_betrayers_prison( special_effect_t& effect )
{
  demon_hunter_t* s = debug_cast<demon_hunter_t*>( effect.player );
  init_special_effect( s, DEMON_HUNTER_VENGEANCE, s -> legendary.fragment_of_the_betrayers_prison, effect );
}

static void loramus_thalipedes_sacrifice( special_effect_t& effect )
{
  demon_hunter_t* s = debug_cast<demon_hunter_t*>( effect.player );
  init_special_effect( s, DEMON_HUNTER_HAVOC, s -> legendary.loramus_thalipedes_sacrifice, effect );
}

static void ph_immolation_aura_damage_lowers_cd_of_fiery_brand( special_effect_t& effect )
{
  demon_hunter_t* s = debug_cast<demon_hunter_t*>( effect.player );
  init_special_effect( s, DEMON_HUNTER_VENGEANCE,
    s -> legendary.ph_immolation_aura_damage_lowers_cd_of_fiery_brand, effect );
}

static void moarg_bionic_stabilizers( special_effect_t& effect )
{
  demon_hunter_t* s = debug_cast<demon_hunter_t*>( effect.player );
  init_special_effect( s, SPEC_NONE,
    s -> legendary.moarg_bionic_stabilizers, effect );
}

static void raddons_cascading_eyes( special_effect_t& effect )
{
  demon_hunter_t* s = debug_cast<demon_hunter_t*>( effect.player );
  init_special_effect( s, DEMON_HUNTER_HAVOC, s -> legendary.raddons_cascading_eyes, effect );
}

static void runemasters_pauldrons( special_effect_t& effect )
{
  demon_hunter_t* s = debug_cast<demon_hunter_t*>( effect.player );
  init_special_effect( s, DEMON_HUNTER_VENGEANCE, s -> legendary.runemasters_pauldrons, effect );
}

static void the_defilers_lost_vambraces( special_effect_t& effect )
{
  demon_hunter_t* s = debug_cast<demon_hunter_t*>( effect.player );
  init_special_effect( s, DEMON_HUNTER_VENGEANCE, s -> legendary.the_defilers_lost_vambraces, effect );
}

// MODULE INTERFACE ==================================================

class demon_hunter_module_t : public module_t
{
public:
  demon_hunter_module_t() : module_t( DEMON_HUNTER )
  {}

  player_t* create_player( sim_t* sim, const std::string& name,
                           race_e r = RACE_NONE ) const override
  {
    auto p              = new demon_hunter_t( sim, name, r );
    p -> report_extension = std::unique_ptr<player_report_extension_t>(
      new demon_hunter_report_t( *p ) );
    return p;
  }

  bool valid() const override { return true; }

  void init( player_t* ) const override {}

  void static_init() const override
  {
    unique_gear::register_special_effect( 208827, anger_of_the_halfgiants );
    unique_gear::register_special_effect( 217735, cloak_of_fel_flames );
    unique_gear::register_special_effect( 208985, eternal_hunger );
    unique_gear::register_special_effect( 217496, fragment_of_the_betrayers_prison );
    unique_gear::register_special_effect( 209002, loramus_thalipedes_sacrifice );
    unique_gear::register_special_effect( 210970, ph_immolation_aura_damage_lowers_cd_of_fiery_brand );
    unique_gear::register_special_effect( 208826, moarg_bionic_stabilizers );
    unique_gear::register_special_effect( 215149, raddons_cascading_eyes );
    unique_gear::register_special_effect( 210867, runemasters_pauldrons );
    unique_gear::register_special_effect( 210840, the_defilers_lost_vambraces );
  }

  void register_hotfixes() const override {}

  void combat_begin( sim_t* ) const override {}

  void combat_end( sim_t* ) const override {}
};

}  // UNNAMED NAMESPACE

const module_t* module_t::demon_hunter()
{
  static demon_hunter_module_t m;
  return &m;
}
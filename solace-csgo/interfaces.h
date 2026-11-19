#include "events.h"
#include "resolver.h"
#include "includes.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

class hurt_listener final : IGameEventListener2
{
    void FireGameEvent(IGameEvent* evt) override
    {
        g_resolver.OnHurt(evt);

        if (evt && g.m_local && g.m_interfaces && g.m_interfaces->engine()) {
            int attacker = g.m_interfaces->engine()->get_player_for_user_id(evt->GetInt("attacker"));
            int victim = g.m_interfaces->engine()->get_player_for_user_id(evt->GetInt("userid"));
            int local_idx = g.m_interfaces->engine()->get_local_player();

            if (attacker == local_idx && victim != local_idx) {
                PlaySoundA("C:\\hitsound\\hit.wav", NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
            }
        }
    }

public:
    void init(IGameEventManager2* game_events)
    {
        game_events_ = (game_events);
        game_events_->AddListener(this, "player_hurt", false);
    }
    ~hurt_listener()
    {
        game_events_->RemoveListener(this);
    }

private:
    IGameEventManager2* game_events_;
} hurt_listener;

class weapon_fire_listener final : IGameEventListener2
{
    void FireGameEvent(IGameEvent* evt) override
    {
        g_resolver.OnFire(evt);
    }

public:
    void init(IGameEventManager2* game_events)
    {
        game_events_ = (game_events);
        game_events_->AddListener(this, "weapon_fire", false);
    }
    ~weapon_fire_listener()
    {
        game_events_->RemoveListener(this);
    }

private:
    IGameEventManager2* game_events_;
} weapon_fire_listener;

class impact_listener final : IGameEventListener2 {
	void FireGameEvent( IGameEvent *evt ) override {
		g_resolver.on_impact( evt );
	}
public:
	void init( IGameEventManager2 *game_events ) {
		game_events_ = ( game_events );
		game_events_->AddListener( this, "bullet_impact", false );
	}
	~impact_listener( ) {
		game_events_->RemoveListener( this );
	}
private:
	IGameEventManager2 *game_events_;
} impact_listener;

class start_listener final : public IGameEventListener2 {
	void FireGameEvent( IGameEvent *evt ) override {
		g_resolver.clear( );
		g.m_valid_round = true;
	}
public:
	void init( IGameEventManager2 *game_events ) {
		game_events_ = ( game_events );
		game_events_->AddListener( this, "round_start", false );
	}
	~start_listener( ) {
		game_events_->RemoveListener( this );
	}
private:
	IGameEventManager2 *game_events_;
} start_listener;

class end_listener final : public IGameEventListener2 {
	void FireGameEvent( IGameEvent *evt ) override {
		g.m_valid_round = false;
		g.m_cmds.clear( );
	}
public:
	void init( IGameEventManager2* game_events ) {
		game_events_ = ( game_events );
		game_events_->AddListener( this, "round_end", false );
	}
	~end_listener( ) {
		game_events_->RemoveListener( this );
	}
private:
	IGameEventManager2 *game_events_;
} end_listener;

void events::init()
{
    weapon_fire_listener.init(g.m_interfaces->events() );
    impact_listener.init(g.m_interfaces->events());
    hurt_listener.init(g.m_interfaces->events());
    start_listener.init(g.m_interfaces->events());
    end_listener.init(g.m_interfaces->events());
}

void events::destroy( ) {
	hurt_listener.~hurt_listener( );
	impact_listener.~impact_listener( );
	start_listener.~start_listener( );
	end_listener.~end_listener( );
    weapon_fire_listener.~weapon_fire_listener();
}

void events::player_hurt( IGameEvent *evt ) {
	g_resolver.OnHurt( evt );
}

void events::bullet_impact( IGameEvent *evt ) {
	g_resolver.on_impact( evt );
}

void events::round_start( IGameEvent *evt ) {
	g_resolver.clear( );
	g.m_valid_round = true;
}

void events::round_end ( IGameEvent *evt ) {
	g.m_valid_round = false;
}

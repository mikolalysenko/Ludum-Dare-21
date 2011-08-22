
//A generic hostile spiky monster
MonsterEntity* spike_monster(IntrinsicCoordinate const& coord) {
	return new MonsterEntity(
		get_artwork("spikeball"),
		Particle(
			coord,
			Vector3f(0, 0, 0),
			Quaternionf(1, 0, 0, 0),
			1.f,
			1.2f),
		MONSTER_FLAG_COLLIDES | 
		MONSTER_FLAG_DEADLY |
		MONSTER_FLAG_CHASE,
		0,
		3.0,
		20.0,
		1.);
}


//A spike monster with a patrol route
MonsterEntity* patrol_spike_monster(vector<IntrinsicCoordinate> const& patrol_route) {

	auto monster = new MonsterEntity(
		get_artwork("spikeball"),
		Particle(
			patrol_route[0],
			Vector3f(0, 0, 0),
			Quaternionf(1, 0, 0, 0),
			2.f,
			1.2f),
		MONSTER_FLAG_COLLIDES | 
		MONSTER_FLAG_DEADLY |
		MONSTER_FLAG_CHASE |
		MONSTER_FLAG_PATROL,
		0,
		3.0,
		25.0,
		1.);
		
	monster->patrol_points = patrol_route;
	
	return monster;
}


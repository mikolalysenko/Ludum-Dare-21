
//A generic hostile spiky monster
MonsterEntity* spike_monster(IntrinsicCoordinate const& coord) {
	return new MonsterEntity(
		get_artwork("spikeball"),
		Particle(
			coord,
			Vector3f(0, 0, 0),
			Quaternionf(1, 0, 0, 0),
			2.f,
			1.2f),
		MONSTER_FLAG_COLLIDES | 
		MONSTER_FLAG_DEADLY |
		MONSTER_FLAG_CHASE,
		0,
		3.0,
		20.0,
		1.);
}



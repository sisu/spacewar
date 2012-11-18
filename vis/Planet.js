function Planet(x, y, z, size, pop, owner) {
	this.pos = vec3(x,y,z);
	this.owner = owner;
	this.size = size;
	this.population = pop;
}

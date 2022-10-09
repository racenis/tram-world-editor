namespace Editor {
  typedef int worldTreeID_t
  void Init();
  void Yeet();


  namespace WorldTree {
    worldTreeID_t Add();
    void Yeet(worldTreeID_t id);
    void Rename (worldTreeID_t id, std::string new_name);
  }

  class Action {
    void Perform() = 0;
    void Unperform() = 0;
  };
}

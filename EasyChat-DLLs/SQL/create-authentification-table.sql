CREATE TABLE Authentification (
  id INTEGER PRIMARY KEY,
  ip TEXT NOT NULL,
  status TEXT NOT NULL,
  timestamp datetime,
  user_id int
);

ALTER TABLE Authentification ADD FOREIGN KEY (user_id) REFERENCES User (id);

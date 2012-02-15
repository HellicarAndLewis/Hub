<?php
class Database {
	private $db = null;
	
	function __construct() {
	}
	
	public function connect($host, $database, $username, $password) {
		$this->db = new PDO(
			'mysql:host=' .$host
			.';dbname=' .$database 
			,$username
			,$password
			,array(1002 => "SET NAMES utf8") 
		);
	}
	
	/**
	 * Insert a new entry in the given table.
	 *
	 * @param	string			Name of the table
	 * @param	array			Array with key-value pairs.
	 * @return	boolean			true when inserted, else false.
	 */
	public function insert($table, $fieldData) {
		// prepare sql
		$sql = 'insert into ' .$table .' (';
		foreach($fieldData as $field => $value) {
			$fields[] = $field;
			$values[] = ':' .$field; 
		}
		$sql .= implode(',', $fields);
		$sql .=') values ( ';
		$sql .= implode(',', $values);
		$sql .= ')';
		
		$query = $this->getDB()->prepare($sql);
		if(!$query->execute($fieldData)) {
			$info = $query->errorInfo();
			l($info[2]);
			return false;
		}
		return true;
	}
	
	private function getDB() {
		return $this->db;
	}
	
	public function lastID() {
		return $this->getDB()->lastInsertId();
	}
	
	public function query($sql) {
		return $this->getDB()->query($sql);
	}
	
	public function execute($query) {
		if(!$query->execute()) {
			$info = $query->errorInfo();
			die($info[2]);
			return false;
		}
		return $query->fetchAll(PDO::FETCH_ASSOC);
	}
	
	
	/**
	 * Fetch one field
	 *
	 */
	private function fetchOne($sql, $values) {
		$query = $this->getDB()->prepare($sql);
		$query->setFetchMode(PDO::FETCH_ASSOC);
		if(!$query->execute($values)) {
			$info = $query->errorInfo();
			l($info[2]);
			return false;
		}
		return $query->fetch();
	}
	
	/** 
	 * Fetch all entries.
	 * 
	 * @param	string		Name of the table to fetch from.
	 * @param	sql-string	Select fields, like: 'first_name, last_name, id'
	 * @param	array		Array with field=value pairs.
	 * return	array		False, or array with entries.
	 */
	private function fetchAll($table, $fieldString, $whereFields) {
		$sql = 'select ' .$fieldString .' from ' .$table;
		if(is_array($whereFields)) {
			foreach($whereFields as $field => $values) {
				$where[] = $this->arraySQL($field, $values);
			}
			$sql .= ' where ' .implode(' AND ', $where);
		}
		$query = $this->getDB()->prepare($sql);
		if(!$query->execute($where)) {
			$info = $query->errorInfo();
			l($info[2]);
			return false;
		}
		return $query->fetchAll(PDO::FETCH_ASSOC);
	}
	
	
	public function arraySQL($field, $values) {
		if(is_array($values) && count($values) > 0) {
			$sql = $field .' IN("' .implode('","', $values) .'")';
		}
		else {
			$sql = $field .' = "' .$values .'"';
		}
		return $sql;
	}
	
	/**
	 * Fetch all the values for the given table and column
	 *
	 * @param	string		Table to query
	 * @param	string		Only return this field.
	 */
	private function fetchColumn($table, $column) {
		$sql = 'select ' .$column .' from ' .$table;
		$query = $this->getDB()->prepare($sql);
		if(!$query->execute()) {
			$info = $query->errorInfo();
			l($info[2]);
			return false;
		}
		return $query->fetchAll(PDO::FETCH_COLUMN);
	}
};

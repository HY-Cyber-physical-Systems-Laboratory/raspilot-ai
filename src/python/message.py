from abc import ABC, abstractmethod
from typing import Any, Dict, List, Optional, Tuple


class MessageBase(ABC):
    """
    Abstract base class for message classes.
    This class defines the interface for message classes that can be used to perform various operations.
    """
    def __init__(self, name: str):
        """
        Initialize the message class with a name.

        Args:
            name (str): The name of the message.
        """
        self.name = name
        self.msg_file = None
        self.msg_type = None
        self.msg = None


    #@abstractmethod
    